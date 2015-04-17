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

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdlib.h>
#include <fstream>
#include <sstream>

#ifndef WIN32
#include <unistd.h>
#endif

#include "IAppl.h"
#include "IApplVNodes.h"
#include "IFilterForward.h"
#include "IGlue.h"
#include "IMessage.h"
#include "IObjectFactory.h"
#include "IScene.h"
#include "ITLparser.h"
#include "OSCAddress.h"
#include "Updater.h"
#include "ip/NetworkingUtils.h"
#include "TMessageEvaluator.h"
#include "ITLError.h"
#include "Tools.h"

#include "INScore.h"

#include <QDir>
#include <QApplication>

#ifdef WIN32
#include <Winsock2.h>
#endif

using namespace std;

namespace inscore
{

//--------------------------------------------------------------------------
// ip address utility
//--------------------------------------------------------------------------
static string getHostName()
{
	char name[512];
	int ret = gethostname(name, 512);
	if (ret == -1) return "";
	return name;
}

string getIP()
{
	string name = getHostName();
	stringstream ipStr;
	if (name.size()) {
		unsigned long ip = GetHostByName(name.c_str());
		ipStr	<< ((ip >> 24) & 0xff) << '.'
				<< ((ip >> 16) & 0xff) << '.'
				<< ((ip >> 8) & 0xff) << '.'
				<< (ip & 0xff);
	}
	return ipStr.str();
}

//--------------------------------------------------------------------------
const string IAppl::kApplType("appl");

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE
std::string IAppl::fRootPath = std::string(getenv("USERPROFILE")) + "\\";
#elif ANDROID
// File are writed in sdcard only
std::string IAppl::fRootPath = "/sdcard/inscore/";
#else
std::string IAppl::fRootPath = std::string(getenv("HOME")) + "/";
#endif


#ifdef RUNBENCH
//--------------------------------------------------------------------------
MsgHandler::msgStatus IAppl::writeBench(const IMessage* msg)
{
	if (msg->size() == 1) {
		string file;
		if (!msg->param(0, file)) return MsgHandler::kBadParameters;
		if (file.size()) {
			bench::write (absolutePath(file));
			return MsgHandler::kProcessed;
		}
	}
	return MsgHandler::kBadParameters;
}

//--------------------------------------------------------------------------
void IAppl::resetBench()
{
	const char *benchnames[] = { "time", "model", "lmap", "smap", "view", "total", 0 };
	unsigned short size = 3000;
	bench::init (benchnames, size);
	oscerr << OSCStart("Bench initialized with buffer size = ") << size << OSCEnd();
}

#endif

//--------------------------------------------------------------------------
bool IAppl::fDefaultShow(true);
bool IAppl::fRunning(true);
const string IAppl::kName = "ITL";
map<string, pair<string, string> > IAppl::fAliases;

string	IAppl::fVersion;						// the application version number
float	IAppl::fVersionNum = 0.;				// the application version number as floating point value
float	IAppl::fCompatibilityVersionNum = 0.;		// the supported version number as floating point value

//--------------------------------------------------------------------------
IAppl::IAppl(int udpport, int outport, int errport,  QApplication* appl, bool offscreen) 
	: IObject(kName, 0), fCurrentTime(0), fCurrentTicks(0), 
	fOffscreen(offscreen), fUDP(udpport,outport,errport), fRate(10), fAppl(appl)
{
	fTypeString = kApplType;
	fVersion	= INScore::versionStr();
	fVersionNum = INScore::version();
	fCompatibilityVersionNum = fVersionNum;
	fStartTime = getTime() / 1000;

	fMsgHandlerMap[khello_SetMethod]			= TMethodMsgHandler<IAppl, void (IAppl::*)() const>::create(this, &IAppl::helloMsg);
//	fMsgHandlerMap["activate"]					= TMethodMsgHandler<IAppl, void (IAppl::*)() const>::create(this, &IAppl::activate);
	fMsgHandlerMap[kload_SetMethod]				= TMethodMsgHandler<IAppl>::create(this, &IAppl::loadMsg);
	fMsgHandlerMap[kread_SetMethod]				= TMethodMsgHandler<IAppl>::create(this, &IAppl::loadBuffer);
	fMsgHandlerMap[krequire_SetMethod]			= TMethodMsgHandler<IAppl>::create(this, &IAppl::requireMsg);
	fMsgHandlerMap[kquit_SetMethod]				= TMethodMsgHandler<IAppl, void (IAppl::*)()>::create(this, &IAppl::quit);
	fMsgHandlerMap[kmouse_SetMethod]			= TMethodMsgHandler<IAppl>::create(this, &IAppl::cursor);
	fMsgHandlerMap[kforward_GetSetMethod]		= TMethodMsgHandler<IAppl>::create(this, &IAppl::forward);
	fMsgHandlerMap[ktime_GetSetMethod]			= TMethodMsgHandler<IAppl>::create(this, &IAppl::setTime);
	fMsgHandlerMap[kticks_GetSetMethod]			= TMethodMsgHandler<IAppl>::create(this, &IAppl::setTicks);
	fMsgHandlerMap[krootPath_GetSetMethod]		= TSetMethodMsgHandler<IAppl, string>::create(this, &IAppl::setRootPath);

	fMsgHandlerMap[kcompatibility_GetSetMethod]		= TSetMethodMsgHandler<IAppl,float>::create(this, &IAppl::setCompatibilityVersion);
	fMsgHandlerMap[kport_GetSetMethod]			= TSetMethodMsgHandler<IAppl,int>::create(this, &IAppl::setUDPInPort);
	fMsgHandlerMap[koutport_GetSetMethod]		= TSetMethodMsgHandler<IAppl,int>::create(this, &IAppl::setUDPOutPort);
	fMsgHandlerMap[kerrport_GetSetMethod]		= TSetMethodMsgHandler<IAppl,int>::create(this, &IAppl::setUDPErrPort);
	fMsgHandlerMap[kdefaultShow_GetSetMethod]	= TSetMethodMsgHandler<IAppl,bool>::create(this, &IAppl::setDefaultShow);
	fMsgHandlerMap[krate_GetSetMethod]			= TSetMethodMsgHandler<IAppl,int>::create(this, &IAppl::setRate);

	fGetMsgHandlerMap[krootPath_GetSetMethod]	= TGetParamMsgHandler<string>::create(fRootPath);
	fGetMsgHandlerMap[kport_GetSetMethod]		= TGetParamMethodHandler<IAppl, int (IAppl::*)() const>::create(this, &IAppl::getUDPInPort);
	fGetMsgHandlerMap[koutport_GetSetMethod]	= TGetParamMethodHandler<IAppl, int (IAppl::*)() const>::create(this, &IAppl::getUDPOutPort);
	fGetMsgHandlerMap[kerrport_GetSetMethod]	= TGetParamMethodHandler<IAppl, int (IAppl::*)() const>::create(this, &IAppl::getUDPErrPort);
	fGetMsgHandlerMap[kdefaultShow_GetSetMethod]= TGetParamMethodHandler<IAppl, bool (IAppl::*)() const>::create(this, &IAppl::defaultShow);
	fGetMsgHandlerMap[kcompatibility_GetSetMethod]	= TGetParamMsgHandler<float>::create(fCompatibilityVersionNum);
	fGetMsgHandlerMap[krate_GetSetMethod]		= TGetParamMethodHandler<IAppl, int (IAppl::*)() const>::create(this, &IAppl::getRate);
	fGetMsgHandlerMap[kforward_GetSetMethod]	= TGetParamMsgHandler<vector<IMessage::TUrl> >::create(fForwardList);
	fGetMsgHandlerMap[ktime_GetSetMethod]		= TGetParamMethodHandler<IAppl, int (IAppl::*)() const>::create(this, &IAppl::time);
	fGetMsgHandlerMap[kticks_GetSetMethod]		= TGetParamMethodHandler<IAppl, int (IAppl::*)() const>::create(this, &IAppl::ticks);

	fGetMsgHandlerMap[kversion_GetMethod]		= TGetParamMsgHandler<string>::create(fVersion);
	fGetMsgHandlerMap["guido-version"]			= TGetParamMethodHandler<IAppl, string (IAppl::*)() const>::create(this, &IAppl::guidoversion);
	fGetMsgHandlerMap["musicxml-version"]		= TGetParamMethodHandler<IAppl, string (IAppl::*)() const>::create(this, &IAppl::musicxmlversion);

#ifdef RUNBENCH
	fMsgHandlerMap[kstartBench_SetMethod]		= TMethodMsgHandler<IAppl, void (IAppl::*)()>::create(this, &IAppl::startBench);
	fMsgHandlerMap[kstopBench_SetMethod]		= TMethodMsgHandler<IAppl, void (IAppl::*)()>::create(this, &IAppl::stopBench);
	fMsgHandlerMap[kresetBench_SetMethod]		= TMethodMsgHandler<IAppl, void (IAppl::*)()>::create(this, &IAppl::resetBench);
	fMsgHandlerMap[kwriteBench_SetMethod]		= TMethodMsgHandler<IAppl>::create(this, &IAppl::writeBench);
#endif
}

//--------------------------------------------------------------------------
IAppl::~IAppl() {}
bool IAppl::oscDebug() const								{ return fApplDebug->getOSCDebug(); }

//--------------------------------------------------------------------------
string IAppl::checkRootPath(const std::string& s)
{
	if (!Tools::isurl(s) && !QDir( QString::fromUtf8(s.c_str()) ).exists() )
		ITLErr << "rootPath is an invalid location:" << s << ITLEndl;
	string root = s;
	char end = root[root.length()-1];
	if (end == '\\')   root[root.length()-1] = '/';
	else if (end != '/') root += '/';
	return root;
}

//--------------------------------------------------------------------------
void IAppl::setReceivedOSC(int n)		{ fApplStat->count(n); }

//--------------------------------------------------------------------------
void IAppl::setRootPath(const std::string& s)
{
	IAppl::fRootPath = checkRootPath(s);
}

//--------------------------------------------------------------------------
void IAppl::print (ostream& out) const
{
	IObject::print (out);
	out << "  running: " << (running() ? "true" : "false") << endl;
	out << "  root path: " << fRootPath << endl;
	out << "  udp infos: " << fUDP << endl;
	out << "  debug: " << fApplDebug << endl;
}

//--------------------------------------------------------------------------
void IAppl::createVirtualNodes()
{
	fApplDebug = IApplDebug::create(this);
	fApplStat  = IApplStat::create(this);					// statistics
	fDebug = fApplDebug;
	fApplLog = IApplLog::create(this);
	fFilterForward = IFilterForward::create(this);
	add ( fDebug );
	add ( fApplStat );
	add ( fApplLog );
	add ( IApplPlugin::create(this) );
	add (fFilterForward);
}

//--------------------------------------------------------------------------
void IAppl::ptask ()
{
	for (unsigned int i = 0; i < elements().size(); i++) {
        elements()[i]->ptask();
	//	IScene* scene = dynamic_cast<IScene*>((IObject*)elements()[i]);
	//	if (scene) scene->ptask();
	}
}

//--------------------------------------------------------------------------
void IAppl::accept (Updater* u)
{
	u->updateTo(SIAppl(this));
}

//--------------------------------------------------------------------------
// the 'get' at root level
// applications parameters are flushed first since
// next messages may depend on the application state
SIMessageList IAppl::getAll() const
{
	SIMessageList outMsgs = getParams();
	for (unsigned int i = 0; i < elements().size(); i++) {
		nodePtr elt = elements()[i];
		if (!elt->getDeleted())
			outMsgs->list().push_back (elt->getAll()->list());
	}
	return outMsgs;
}

//--------------------------------------------------------------------------
// messages processing
//--------------------------------------------------------------------------
bool IAppl::filter (const IMessage* msg)
{
	if (msg->message() == kforward_GetSetMethod) return true;

	return fFilterForward->applyFilter(msg);
}

//--------------------------------------------------------------------------
int IAppl::processMsg (const std::string& address, const std::string& addressTail, const IMessage* imsg)
{
	setReceivedOSC (1);

	int n = fForwardList.size();

	if (imsg->extendedAddress()) {
		OSCStream::sendEvent (imsg, imsg->url().fHostname, imsg->url().fPort);
		return MsgHandler::kProcessed;
	}
	else if (n && !filter(imsg)) {
		for (int i = 0; i < n; i++) {
			IMessage::TUrl url = fForwardList[i];
			OSCStream::sendEvent (imsg, url.fHostname, url.fPort);
		}
	}

	string head = address;
	string tail = addressTail;
	SIMessage msg = IMessage::create (*imsg);
	TAliasesMap::const_iterator i = fAliases.find(imsg->address());
	if (i != fAliases.end()) {
		msg->setAddress (i->second.first);
		if (i->second.second.size()) 
			msg->setMessage(i->second.second);
		head = OSCAddress::addressFirst(i->second.first);
		tail = OSCAddress::addressTail(i->second.first);
	}

	if (tail.size()) 		// application is not the final destination of the message
		return IObject::processMsg(head, tail, msg);
	
	if (match(head)) {			// the message is for the application itself
		int status = execute(msg);
		if (status & MsgHandler::kProcessed)
			setState(IObject::kModified);
		return status;
	}
	return MsgHandler::kBadAddress;
}

//--------------------------------------------------------------------------
// messages handlers
//--------------------------------------------------------------------------
SIMessage IAppl::hello()	const
{
	SIMessage msg = IMessage::create (getOSCAddress());
	*msg << getIP() << getUDPInPort() << getUDPOutPort() << getUDPErrPort();
	return msg;
}

//--------------------------------------------------------------------------
void IAppl::helloMsg() const
{
	SIMessage msg = hello();
	msg->print(oscout);
}

//--------------------------------------------------------------------------
//void IAppl::activate() const
//{
//	ITLErr << "activate "  << ITLEndl;
//	fAppl->postEvent (fAppl, new QEvent(QEvent::ApplicationActivate));
//}

//--------------------------------------------------------------------------
string IAppl::guidoversion() const
{
	return INScore::guidoversion();
}

//--------------------------------------------------------------------------
string IAppl::musicxmlversion() const
{
	return INScore::musicxmlversion();
}

//--------------------------------------------------------------------------
MsgHandler::msgStatus IAppl::requireMsg(const IMessage* msg)
{
	if (msg->size() > 0) {
		float version = INScore::version();
		float required;
		if (msg->param(0, required)) {
			if (version >= required) {
				return MsgHandler::kProcessed;
			}
			else {
				ITLErr << "Version" << required << "is required: current version is" << version  << ITLEndl;
				if (msg->size() > 1) {
					const IMessageList* msgs = msg->watchMsg2Msgs(1);
					if (!msgs || msgs->list().empty()) return MsgHandler::kBadParameters;

					MouseLocation mouse (0, 0, 0, 0, 0, 0);
					EventContext env(mouse, libmapping::rational(0,1), this);
					TMessageEvaluator me;
					SIMessageList outmsgs = me.eval (msgs, env);
					if (outmsgs && outmsgs->list().size()) outmsgs->send();
				}
			}
			return MsgHandler::kProcessed;
		}
	}
	return MsgHandler::kBadParameters;
}

//--------------------------------------------------------------------------
MsgHandler::msgStatus IAppl::forward(const IMessage* msg)
{
	fForwardList.clear();						// clear the forward list first
	if (msg->size() == 0)						// no other param
		return MsgHandler::kProcessed;			// that's done

	for (int i=0; i<msg->size(); i++) {
		string address;
		if (msg->param(i, address)) {
			IMessage::TUrl url;
			url.parse (address);
			if (!url.fPort) url.fPort = getUDPInPort();
			fForwardList.push_back (url);
		}
		else return MsgHandler::kBadParameters;
	}
	return MsgHandler::kProcessed;
}

//--------------------------------------------------------------------------
void IAppl::clock()
{
	QMutexLocker locker (&fTimeMutex);
	fCurrentTime = getTime() / 1000 - fStartTime;
	fCurrentTicks++;
}

//--------------------------------------------------------------------------
MsgHandler::msgStatus IAppl::setTime(const IMessage* msg)
{
	if (msg->size() == 1) {
		int time;
		if (msg->param(0, time)) {
			QMutexLocker locker (&fTimeMutex);
			fStartTime = getTime() / 1000 + time;
			return MsgHandler::kProcessed;
		}
	}
	return MsgHandler::kBadParameters;
}

//--------------------------------------------------------------------------
MsgHandler::msgStatus IAppl::setTicks(const IMessage* msg)
{
	if (msg->size() == 1) {
		int ticks;
		if (msg->param(0, ticks)) {
			QMutexLocker locker (&fTimeMutex);
			fCurrentTicks = ticks;
			return MsgHandler::kProcessed;
		}
	}
	return MsgHandler::kBadParameters;
}

//--------------------------------------------------------------------------
MsgHandler::msgStatus IAppl::cursor(const IMessage* msg)
{
	if (!fAppl) return MsgHandler::kProcessed;
	if (msg->size() == 1) {
		string status;
		if (msg->param(0, status)) {
			if (status == "hide") 
				fAppl->setOverrideCursor( QCursor( Qt::BlankCursor ) );
			else
				fAppl->setOverrideCursor( QCursor( Qt::ArrowCursor ) );
			return MsgHandler::kProcessed;
		}
	}
	return MsgHandler::kBadParameters;
}

//--------------------------------------------------------------------------
MsgHandler::msgStatus IAppl::loadBuffer (const IMessage* msg)
//bool IAppl::loadBuffer (const char* buffer)
{
	stringstream s;
	for (int i=0; i<msg->size(); i++ ) {
		string str; int ivalue; float fvalue;
		if (msg->param(i, str)) s << str << " ";
		else if (msg->param(i, ivalue)) s << ivalue << " ";
		else if (msg->param(i, fvalue)) s << fvalue << " ";
	}
	if (!s.str().size()) return MsgHandler::kBadParameters;

	ITLparser p (&s, 0, &fJavascript, &fLua);
	SIMessageList msgs = p.parse();
	if (msgs) {
		for (IMessageList::TMessageList::const_iterator i = msgs->list().begin(); i != msgs->list().end(); i++) {
			string beg  = OSCAddress::addressFirst((*i)->address());
			string tail = OSCAddress::addressTail((*i)->address());
			int ret = processMsg(beg, tail, *i);
			if (oscDebug()) IGlue::trace(*i, ret);
		}
		return MsgHandler::kProcessed;
	}
	return MsgHandler::kBadParameters;
}

//--------------------------------------------------------------------------
MsgHandler::msgStatus IAppl::loadMsg(const IMessage* msg)
{
	return load (msg, this, getRootPath());
}

//--------------------------------------------------------------------------
std::string IAppl::absolutePath( const std::string& path )
{
	return TILoader::makeAbsolutePath (getRootPath(), path);
}

//--------------------------------------------------------------------------
void IAppl::addAlias( const string& alias, const std::string& address, const std::string& msg )
{
	fAliases[alias] = make_pair(address, msg);
}

//--------------------------------------------------------------------------
void IAppl::delAliases( const string& address)
{
	TAliasesMap::iterator i = fAliases.begin();
	while ( i != fAliases.end() ) {
		if (i->second.first == address) {
			TAliasesMap::iterator j = i;
			i++;
			fAliases.erase (j);
		}
		else i++;
	}
}

//--------------------------------------------------------------------------
void IAppl::getAliases( const string& address, vector<pair<string, string> >& aliases)
{
	TAliasesMap::iterator i = fAliases.begin();
	while ( i != fAliases.end() ) {
		if (i->second.first == address) {
			aliases.push_back(make_pair(i->first, i->second.second));
		}
		i++;
	}
}

}