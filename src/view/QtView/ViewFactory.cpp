/*

  INScore Project
  Copyright (C) 2010  Grame

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

  Grame Research Laboratory, 11 cours de Verdun Gensoul, 69002 Lyon - France
  research@grame.fr

*/

#include <QGraphicsScene>

#include "IModel.h"
#include "ViewFactory.h"
#include "VObjectView.h"

#include "VApplView.h"
#include "VArcView.h"
#include "VAudioView.h"
#include "VCurveView.h"
#include "VEllipseView.h"
#include "VGestureFollowerView.h"
#include "VGraphView.h"
#include "VGuidoItemView.h"
#include "VImageView.h"
#include "VLineView.h"
#include "VPolygonView.h"
#include "VRectView.h"
#include "VGridView.h"
#include "VLayerView.h"
#include "VPianoRollView.h"
#include "VSceneView.h"
#include "VSVGView.h"
#include "VTextView.h"
#include "VHttpdView.h"
#include "VVideoView.h"
#include "VUrlIntermediateObjectView.h"

namespace inscore
{

//--------------------------------------------------------------------------
VObjectView*	ViewFactory::create (const IArc* object,		QGraphicsScene* scene)		{ return new VArcView (scene, object); }
VObjectView*	ViewFactory::create (const IAudio* object,		QGraphicsScene* scene)		{ return new VAudioView (scene, object); }
VObjectView*	ViewFactory::create (const ICurve* object,		QGraphicsScene* scene)		{ return new VCurveView (scene, object); }
VObjectView*	ViewFactory::create (const IEllipse* object,	QGraphicsScene* scene)		{ return new VEllipseView (scene, object); }
VObjectView*	ViewFactory::create (const IGestureFollower* object,  QGraphicsScene* scene){ return new VGestureFollowerView (scene, object); }
VObjectView*	ViewFactory::create (const IGraphicSignal* object,  QGraphicsScene* scene)	{ return new VGraphView (scene, object); }
VObjectView*	ViewFactory::create (const ISGraphicSignal* object, QGraphicsScene* scene)	{ return new VSGraphView (scene, object); }
VObjectView*	ViewFactory::create (const IRGraphicSignal* object, QGraphicsScene* scene)	{ return new VRGraphView (scene, object); }
VObjectView*	ViewFactory::create (const IGuidoCode* object,	QGraphicsScene* scene)		{ return new VGuidoItemView (scene, object); }
VObjectView*	ViewFactory::create (const IGuidoPianoRoll* object,	QGraphicsScene* scene)	{ return new VPianoRollView (scene, object); }
VObjectView*	ViewFactory::create (const IGuidoPianoRollStream* object, QGraphicsScene* scene) { return new VPianoRollView (scene, object); }
VObjectView*	ViewFactory::create (const IGuidoFile* object,	QGraphicsScene* scene)		{ return new VGuidoItemView (scene, object); }
VObjectView*	ViewFactory::create (const IGuidoPianoRollFile* object,	QGraphicsScene* scene) { return new VPianoRollView (scene, object); }
VObjectView*	ViewFactory::create (const IMusicXMLCode* object, QGraphicsScene* scene)	{ return new VGuidoItemView (scene, object); }
VObjectView*	ViewFactory::create (const IMusicXMLFile* object, QGraphicsScene* scene)	{ return new VGuidoItemView (scene, object); }
VObjectView*	ViewFactory::create (const IHtml* object,		QGraphicsScene* scene)		{ return new VTextView (scene, object); }
VObjectView*	ViewFactory::create (const IHtmlFile* object,	QGraphicsScene* scene)		{ return new VTextView (scene, object); }
VObjectView*	ViewFactory::create (const IHttpd* object,		QGraphicsScene* scene)		{ return new VHttpdView (scene, object); }
VObjectView*	ViewFactory::create (const IImage* object,		QGraphicsScene* scene)		{ return new VImageView (scene, object); }
VObjectView*	ViewFactory::create (const IMemImage* object,	QGraphicsScene* scene)		{ return new VImageView (scene, object); }
VObjectView*	ViewFactory::create (const ILine* object,		QGraphicsScene* scene)		{ return new VLineView (scene, object); }
VObjectView*	ViewFactory::create (const IPolygon* object,	QGraphicsScene* scene)		{ return new VPolygonView (scene, object); }
VObjectView*	ViewFactory::create (const IGrid* object,		QGraphicsScene* scene)		{ return new VGridView (scene, object); }
VObjectView*	ViewFactory::create (const IRect* object,		QGraphicsScene* scene)		{ return new VRectView (scene, object); }
VObjectView*	ViewFactory::create (const ISVG* object,		QGraphicsScene* scene)		{ return new VSVGView (scene, object); }
VObjectView*	ViewFactory::create (const ISVGFile* object,	QGraphicsScene* scene)		{ return new VSVGView (scene, object); }
VObjectView*	ViewFactory::create (const IText* object,		QGraphicsScene* scene)		{ return new VTextView (scene, object); }
VObjectView*	ViewFactory::create (const ITextFile* object,	QGraphicsScene* scene)		{ return new VTextView (scene, object); }
VObjectView*	ViewFactory::create (const IVideo* object,		QGraphicsScene* scene)		{ return new VVideoView (scene, object); }
VObjectView*    ViewFactory::create (const ILayer* object,      QGraphicsScene* scene)      { return new VLayerView (scene, object);}
VObjectView*	ViewFactory::create (const IUrlIntermediateObject* object, QGraphicsScene* scene){return new VUrlIntermediateObjectView (scene, object);}

//--------------------------------------------------------------------------
VObjectView* ViewFactory::create(const IScene* obj)
{
	VSceneView * scene = new VSceneView ();
    scene->initializeView(obj->getOSCAddress(), new QGraphicsScene);
	return scene;
}

VObjectView* ViewFactory::create(const IAppl* )			{ return new VApplView (); }

}

