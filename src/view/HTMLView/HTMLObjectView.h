/*

  INScore Project

  Copyright (C) 2009,2020  Grame

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


#pragma once

#include <iostream>

#include "IObject.h"
#include "VObjectView.h"


namespace inscore
{

/*!
\addtogroup ITLView 
@{
*/

//--------------------------------------------------------------------------
/**
*	\brief the SVG graphic view of an IObject.
*/
class SVGObjectView :  public VObjectView
{
	public :
				 SVGObjectView()  {}
		virtual ~SVGObjectView()  {}
		
		virtual void updateView(IObject * object)	{ std::cout << "SVGObjectView::updateView " << object->name() << std::endl; }

		virtual void updateObjectSize( IObject *  )	{}
		virtual void setParentItem( VObjectView*  )	{}

		virtual void setEffect (const IEffect* effect )	{}

		virtual TFloatRect referenceRect(VObjectContext item) const { return TFloatRect(TFloatPoint(-1,-1), TFloatPoint(1,1)); };
};

/*!@} */

} // end namespoace
