/*

  INScore Project

  Copyright (C) 2009,2010  Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/

#include <iostream>
#include <fstream>
#include "deelx.h"

#include "IAppl.h"
#include "IGlue.h"
#include "IGraphicSignal.h"
#include "IMessage.h"
#include "IObjectFactory.h"
#include "ISignalNode.h"
#include "IScene.h"
#include "ISceneSync.h"
#include "ITLError.h"
#include "ITLparser.h"
#include "OSCAddress.h"
#include "QFileWatcher.h"
#include "rational.h"
#include "TMessageEvaluator.h"
#include "Updater.h"
#include "IJavascript.h"
#include "IFilterForward.h"

#include "VSceneView.h"

using namespace std;

namespace inscore
{

//--------------------------------------------------------------------------
const string IScene::kSceneType("scene");

//--------------------------------------------------------------------------
IScene::~IScene() 
{
    for(int i=0; i<size(); i++)
        elements()[i]->del();
    fSync->cleanup();
    elements().clear();		// this is required to avoid orphan QGraphicsItem (and crash after that)
}

IScene::IScene(const std::string& name, IObject * parent) 
		: IRectShape(name, parent), fFullScreen(false), fFrameless(false), fAbsoluteCoordinates(false), fWindowOpacity(false), fUpdateVersion(false)
{
	fTypeString = kSceneType;
	setColor( IColor(255,255,255,255) );
	setWidth(1.0f);
	setHeight(1.0f);
	
	fMsgHandlerMap[knew_SetMethod]				= TMethodMsgHandler<IScene, void (IScene::*)(void)>::create(this, &IScene::newScene);
	fMsgHandlerMap[kreset_SetMethod]			= TMethodMsgHandler<IScene, void (IScene::*)(void)>::create(this, &IScene::reset);
	fMsgHandlerMap[kforeground_SetMethod]		= TMethodMsgHandler<IScene, void (IScene::*)(void)>::create(this, &IScene::foreground);
	fMsgHandlerMap[kfullscreen_GetSetMethod]	= TSetMethodMsgHandler<IScene,bool>::create(this,&IScene::setFullScreen);
	fMsgHandlerMap[kframeless_GetSetMethod]		= TSetMethodMsgHandler<IScene,bool>::create(this,&IScene::setFrameless);
	fMsgHandlerMap[kabsolutexy_GetSetMethod]	= TSetMethodMsgHandler<IScene,bool>::create(this,&IScene::setAbsoluteCoordinates);
	fMsgHandlerMap[kwindowOpacity_GetSetMethod]	= TSetMethodMsgHandler<IScene,bool>::create(this,&IScene::setWindowOpacity);
	fMsgHandlerMap[kload_SetMethod]				= TMethodMsgHandler<IScene>::create(this, &IScene::loadMsg);
	fMsgHandlerMap[krootPath_GetSetMethod]		= TSetMethodMsgHandler<IScene, string>::create(this, &IScene::setRootPath);
	fMsgHandlerMap[kforward_GetSetMethod]		= TMethodMsgHandler<IScene>::create(this, &IScene::forward);

	fGetMsgHandlerMap[kfullscreen_GetSetMethod] = TGetParamMsgHandler<bool>::create(fFullScreen);
	fGetMsgHandlerMap[kframeless_GetSetMethod]	= TGetParamMsgHandler<bool>::create(fFrameless);
	fGetMsgHandlerMap[kabsolutexy_GetSetMethod] = TGetParamMsgHandler<bool>::create(fAbsoluteCoordinates);
	fGetMsgHandlerMap[kwindowOpacity_GetSetMethod] = TGetParamMsgHandler<bool>::create(fWindowOpacity);
	fGetMsgHandlerMap[krootPath_GetSetMethod]	= TGetParamMsgHandler<string>::create(fRootPath);
	fGetMsgHandlerMap[kforward_GetSetMethod]	= TGetParamMethodHandler<IScene, const vector<IMessage::TUrl> (IScene::*)() const>::create(this, &IScene::getForwardList);
}

//--------------------------------------------------------------------------
QGraphicsScene * IScene::getGraphicScene () const			{ return fView ? static_cast<VSceneView*>(fView)->scene() : 0; }

//--------------------------------------------------------------------------
void IScene::setHandlers ()
{
	colorAble();
	positionAble();
	fGetMsgHandlerMap[""]			= (void*)0;	// force standard propagation of the get message
	fMsgHandlerMap[keffect_GetSetMethod]		= (void*)0;	// no effects at scene level
	fGetMsgHandlerMap[keffect_GetSetMethod]		= (void*)0;	// no effects at scene level
}

//--------------------------------------------------------------------------
void IScene::newScene ()	{}
void IScene::foreground()	{ getView()->foreground(); }
void IScene::setRootPath(const std::string& s) { fRootPath = IAppl::checkRootPath(s);}
void IScene::del()			{ _del(false); }

//--------------------------------------------------------------------------
void IScene::reset ()
{
	setColor( IColor(255,255,255,255) );
	setXPos(0.f);
	setYPos(0.f);
	setScale(1.0f);
	setXOrigin(0.0f);
	setYOrigin(0.0f);
	setWidth(1.0f);
	setHeight(1.0f);
	signalsNode()->delsubnodes();
	delsubnodes();
	fFileWatcher->clear();
	fRootPath.clear();
	fFullScreen = false; 
	fFrameless = false;
	fJavascript.Initialize();
	fLua.Initialize();
}

//--------------------------------------------------------------------------
void IScene::createVirtualNodes()
{
	IRectShape::createVirtualNodes();

	fFileWatcher = QFileWatcher::create(this);
	fJSObject = IJavascript::create(this, &fJavascript);
	fFilterForward = IFilterForward::create(this);
	fForwarder.setFilter(fFilterForward);
	add ( fFileWatcher );
	add ( fJSObject );
	add ( fFilterForward );
}

//--------------------------------------------------------------------------
string IScene::getRootPath() const
{
	return fRootPath.size() ? fRootPath : IAppl::getRootPath();
}

//--------------------------------------------------------------------------
string IScene::absolutePath(const std::string& path) const
{
	return IAppl::makeAbsolutePath (getRootPath(), path);
}

//--------------------------------------------------------------------------
void IScene::accept (Updater* u)
{
	u->updateTo (SIScene(this));
}

//--------------------------------------------------------------------------
bool IScene::accept(const std::string& regexp, const IMessage *msg)
{
	bool result = IObject::match(regexp);
	if (result)
		fForwarder.forward(msg);
	return result;
}

//--------------------------------------------------------------------------
int	IScene::execute (const IMessage* msg)
{
	fForwarder.forward(msg);
	return IObject::execute(msg);
}

//--------------------------------------------------------------------------
// a message addressed to /ITL/scene that is dropped to another scene is converted
// to the other scene address.
// in a given way, the address /ITL/scene match any scene address
string IScene::address2scene (const char* addr) const
{
//	CRegexpT<char> regexp("/ITL/[^\\/]*", EXTENDED);
	CRegexpT<char> regexp("/ITL/scene", EXTENDED);
	char * replaced = regexp.Replace (addr, getOSCAddress().c_str());
	string sceneAddress (replaced);
	regexp.ReleaseString (replaced);
	return sceneAddress;
}

//--------------------------------------------------------------------------
MsgHandler::msgStatus IScene::loadMsg(const IMessage* msg)
{
	return load (msg, this, getRootPath());
}

//--------------------------------------------------------------------------
void IScene::endPaint () const
{
	const IMessageList* msgs = eventsHandler()->getMessages (EventsAble::kEndPaint);
	if (!msgs || msgs->list().empty()) return;		// nothing to do, no associated message

	MouseLocation mouse (0, 0, 0, 0, 0, 0);
	EventContext env(mouse, libmapping::rational(0,1), 0);
	TMessageEvaluator me;
	SIMessageList outmsgs = me.eval (msgs, env);
	if (outmsgs && outmsgs->list().size()) outmsgs->send();
}

//--------------------------------------------------------------------------
void IScene::add (const nodePtr& node)
{
	IObject::add (node);

	const IMessageList* msgs = eventsHandler()->getMessages (EventsAble::kNewElement);
	if (!msgs || msgs->list().empty()) return;		// nothing to do, no associated message

	MouseLocation mouse (0, 0, 0, 0, 0, 0);
	EventContext env(mouse, libmapping::rational(0,1), node);
	TMessageEvaluator me;
	SIMessageList outmsgs = me.eval (msgs, env);
	if (outmsgs && outmsgs->list().size()) outmsgs->send();
}

//--------------------------------------------------------------------------
void IScene::setState (state s)
{
	IObject::setState(s);
	if (getState() & (IObject::kModified | IObject::kSubModified))
		setUpdateVersion(true);
}

//--------------------------------------------------------------------------
MsgHandler::msgStatus IScene::_watchMsg(const IMessage* msg, bool add)
{ 
	if (msg->size()) {
		string what;
		if (msg->param (0, what)) {
			EventsAble::eventype t = EventsAble::string2type (what);
			switch (t) {
				case EventsAble::kNewElement:
				case EventsAble::kEndPaint:
					if (msg->size() > 1)
						if (add) eventsHandler()->addMsg (t, msg->watchMsg2Msgs(1));
						else eventsHandler()->setMsg (t, msg->watchMsg2Msgs(1));
					else if (!add) eventsHandler()->setMsg (t, 0);
					return MsgHandler::kProcessed;

				default:
					break;
			}
		}
	}
	return IObject::_watchMsg(msg, add);
}

//--------------------------------------------------------------------------
void IScene::print (ostream& out) const
{
	IRectShape::print (out);
	out << "  mode : " << (getFullScreen() ? "normal" : "full") << "screen" << endl;
	out << "  nodes synchronization :" << endl << fSync->getSync();
	out << "  file watcher :" << endl;
	fFileWatcher->print(out);
}

//--------------------------------------------------------------------------
MsgHandler::msgStatus IScene::forward(const IMessage* msg)
{
	return fForwarder.processForwardMsg(msg);
}

}