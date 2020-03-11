/*

  INScore Library
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

#pragma once

#include <iostream>

#include "IObject.h"
#include "INScoreGlue.h"

namespace inscore
{


//--------------------------------------------------------------------------
/*! \brief the main library interface for emcc
*/
class inscore_export IObjectAdapter
{
	SIObject fObject;
	public:
		typedef const std::string	jsString;
	
				 IObjectAdapter() : fObject(0) {}
				 IObjectAdapter(const SIObject& object) : fObject(object) {}
		virtual ~IObjectAdapter() {}
		
		float	getXPos () const			{ return fObject->getXPos(); }
		float	getYPos () const			{ return fObject->getYPos(); }
		float	getXOrigin () const			{ return fObject->getXOrigin(); }
		float	getYOrigin () const			{ return fObject->getYOrigin(); }
		float	getZOrder() const			{ return fObject->getZOrder(); }
		float	getScale () const			{ return fObject->getScale(); }
		TFloatPoint	getPos() const			{ return fObject->getPos(); }
		float	getRotateX() const			{ return fObject->getRotateX(); }
		float	getRotateY() const			{ return fObject->getRotateY(); }
		float	getRotateZ() const			{ return fObject->getRotateZ(); }
		float	getWidth() const			{ return fObject->getWidth(); }
		float	getHeight() const			{ return fObject->getHeight(); }
		TFloatSize getDimension() const		{ return fObject->getDimension(); }
		TFloatSize getShear() const			{ return fObject->getShear(); }
		bool	getVisible () const			{ return fObject->getVisible(); }

		libmapping::rational getDate () const		{ return fObject->getDate(); }
		libmapping::rational getDuration () const	{ return fObject->getDuration(); }

	IObjectAdapter* create(int id) 	{ IObject* o = (IObject*)id; return new IObjectAdapter(o); }

};

}