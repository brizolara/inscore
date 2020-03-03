/*

  INScore Project

  Copyright (C) 2015  Grame

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
#include "Forwarder.h"
#include "IFilterForward.h"
#include "Tools.h"
#include "IAppl.h"

#include <string>

namespace inscore
{

//--------------------------------------------------------------------------
void Forwarder::forward(const IMessage * imsg)
{
#ifndef NO_OSCSTREAM
	// Forward if have host in foward list and if no filter or filter is not bloking the message.
	if (fForwardList.size() && (!fFilter || !fFilter->applyFilter(imsg))) {
        for (unsigned int i = 0; i < fForwardList.size(); i++) {
            IMessage::TUrl url = fForwardList[i];
            // Forward message only if the destination is not the source of the message.
            if(Tools::ip2string(imsg->src()) != url.fHostname)
                OSCStream::sendEvent (imsg, url.fHostname, url.fPort);
        }
    }
#endif
}

//--------------------------------------------------------------------------
MsgHandler::msgStatus Forwarder::processForwardMsg(const IMessage* msg)
{
	fForwardList.clear();					// clear the forward list first
#ifndef NO_OSCSTREAM
	if (msg->size() == 0)					// no other param
		return MsgHandler::kProcessed;		// that's done

    for (int i=0; i<msg->size(); i++) {
        std::string address;
        if (msg->param(i, address)) {
            IMessage::TUrl url;
            url.parse (address);
            // Transform hostname in Ip in string format
            url.fHostname = Tools::ip2string(Tools::getIP(url.fHostname));
			if (!url.fPort) url.fPort = IAppl::kUPDPort;
			// Add in host list.
            fForwardList.push_back(url);
        }
        else return MsgHandler::kBadParameters;
    }
#endif
    return MsgHandler::kProcessed;
}

}
