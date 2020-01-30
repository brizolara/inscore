/*

  INScore Project

  Copyright (C) 2009,2015  Grame

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
#include "IUrlIntermediateObject.h"

namespace inscore
{

class QFileDownloader;
class QUrlIntermediateObject;
typedef class libmapping::SMARTP<QUrlIntermediateObject>	SQUrlIntermediateObject;

class QUrlIntermediateObject : public IUrlIntermediateObject
{
	
    public:
		enum status { kDownloading, kCancelled, kFailed };
		static const std::string kUrlIntermediateType;
        static SQUrlIntermediateObject create(const std::string& name, IObject * parent)	{ return new QUrlIntermediateObject(name, parent); }
    
		virtual void	accept (Updater*);
		status	getStatus()		{ return fStatus; }

    
    protected:    
				 QUrlIntermediateObject( const std::string& name, IObject* parent );
		virtual ~QUrlIntermediateObject() { delete fDownloaderThread; }
    
        virtual MsgHandler::msgStatus set (const IMessage* msg );

		/// \brief override IObject method
		virtual bool acceptSimpleEvent(EventsAble::eventype t) const;
	
        /// \brief sets the type of file that this intermediate object momently replace
		void setType(std::string type) {fType = type; }
    
        /// \brief returns the type of file that this intermediate object momently replace
		std::string getType() { return fType; }
    
        /// \brief handles the case of success ("success" message sent by the QFileDownloader if the download has succeeded)
		virtual void updateFileSucceded();
    
        /// \brief handles the case of cancelation ("cancel" message, or in the case of a change of path or type)
		virtual void updateFileCanceled();
    
        /// \brief handles the case of failure ("error" message sent by the QFileDownloader if the download has failed)
		virtual MsgHandler::msgStatus updateFileFailed(const IMessage* msg );
    	
        std::string fUrlPath;
        std::string fType; // the type of the object to be created once the data is ready
        QFileDownloader *    fDownloaderThread; // the thread called to download the data from the URL. Once done, it sends the appropriate msg
	
		status fStatus;
};
}
