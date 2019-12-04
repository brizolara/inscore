/*

  INScore Project

  Copyright (C) 2009,2011  Grame

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


#ifndef __VObjectView__
#define __VObjectView__

#include <iostream>
#include <vector>
#include <QGraphicsItem>

#include "GraphicEffect.h"
#include "EventsAble.h"
#include "abstractdata.h"


namespace inscore
{

/*!
\addtogroup ITLView 
@{
*/

class IObject;

//--------------------------------------------------------------------------
/**
*	\brief a graphic view of a IObject that uses a QGraphicsItem to render.
*/
class VObjectView
{
	public :
		virtual ~VObjectView()	{}

		virtual void updateView(IObject * object)			= 0;
		virtual void updateObjectSize( IObject * object )	= 0;
		//virtual void setParentView (IObject * object)		= 0;
		virtual void setParentItem( VObjectView* parent )	= 0;

		virtual void setEffect (GraphicEffect& effect)		= 0;
		virtual GraphicEffect getEffect () const			= 0;

		/// \brief Maps the IObject [-1,1] y coordinate to the referenceRect().
		virtual float relative2SceneY(float y, QGraphicsItem * item = 0 ) const						= 0;
		/// \brief Maps the IObject [-1,1] x coordinate to the referenceRect().
		virtual float relative2SceneX(float x, QGraphicsItem * item = 0 ) const						= 0;
		/// \brief Maps the IObject [0,2] width value to the corresponding referenceRect() value.
		virtual float relative2SceneWidth(float width, QGraphicsItem * item = 0 ) const				= 0;
		/// \brief Maps the IObject [0,2] height value to the corresponding referenceRect() value.
		virtual float relative2SceneHeight(float height, QGraphicsItem * item = 0 ) const				= 0;

		/// \brief Maps the referenceRect() width value to the corresponding [0,2] value.
		virtual float scene2RelativeWidth(float width, QGraphicsItem * item = 0 ) const				= 0;
		/// \brief Maps the referenceRect() height value to the corresponding [0,2] value.
		virtual float scene2RelativeHeight(float height, QGraphicsItem * item = 0 ) const				= 0;
		/// \brief Maps the referenceRect() x value to the corresponding [-1,1] value.
		virtual float scene2RelativeX(float x, QGraphicsItem * item = 0 ) const						= 0;
		/// \brief Maps the referenceRect() y value to the corresponding [-1,1] value.
		virtual float scene2RelativeY(float y, QGraphicsItem * item = 0 ) const						= 0;

		/// \brief updates the local mapping (do nothing at IObject level) 
		virtual void	updateLocalMapping (IObject* )					{}
		
		/// \brief initialize an object view (do nothing at IObject level)
		virtual void	initialize (IObject* )							{}
		
		/// \brief apply the current transform matrix to an object rect
		virtual void	getFrame (const IObject* , std::vector<float>&)	{}

		/*!
			\brief set the object view image using another view
			\param src the src view
		*/
		virtual void	setImage (VObjectView* )						{}

		/*!
			\brief set the object view image using another view
			\param src the src view
		*/
		virtual void	setImage (const unsigned char* , int )	{}
	
		/*!
		 * \brief getImage get image data of the object view at specified format.
		 * \param format Image format of the data.
		 * \return data of the image
		 */
		virtual const AbstractData		getImage(const char *);

		/*!
		 * \brief getVersion get the score version.
		 * \return
		 */
		virtual unsigned long	getVersion() const { return 0; }

		/// \brief simulate UI mouse events
		virtual void handleEvent (const IObject* obj, float x, float y, EventsAble::eventype type);

		/// \brief Enable or disable mouse and touch event on this object
		virtual void setMouseEventSensibility(bool){}

        virtual void refreshSyncCache(){}
};

/*!@} */

} // end namespoace

#endif
