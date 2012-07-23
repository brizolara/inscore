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


#ifndef __IAppl__
#define __IAppl__

#include "IMessageHandlers.h"
#include "PeriodicTask.h"
#include "IObject.h"
#include "TScripting.h"
#include "udpinfo.h"
#include "benchtools.h"

class QApplication;
namespace inscore
{

/*!
\addtogroup ITLModel
@{
*/

class IAppl;
typedef class SMARTP<IAppl>	SIAppl;

class IApplDebug;
typedef class SMARTP<IApplDebug> SIApplDebug;

//--------------------------------------------------------------------------
/*!
	\brief the application object of the model
*/
class IAppl : public IObject, public PeriodicTask
{
	typedef std::map<std::string, std::pair<std::string, std::string> >		TAliasesMap;
	static TAliasesMap fAliases;

	static std::string	fRootPath;
	static bool			fRunning;
		
		std::string fVersion;
		SIApplDebug	fApplDebug;	
		bool		fOffscreen;
		udpinfo		fUDP;
		QApplication* fAppl;

		TJSEngine		fJavascript;
		TLua			fLua;

	public:
		static bool fDefaultShow;
		static const std::string kApplType;
		static const std::string kName;
		static SIAppl			create(int udpport, int outport, int errport,  QApplication* appl, bool offscreen=false)		
			{ return new IAppl(udpport, outport, errport, appl, offscreen); }
		static std::string		getRootPath()			{ return fRootPath; }	//< returns the application root path
		static std::string		absolutePath( const std::string& path );		//< returns the absolute path corresponding to 'path',
		static std::string		makeAbsolutePath( const std::string& path, const std::string& file );

		static void				addAlias( const std::string& alias, const std::string& address, const std::string& msg);
		static void				delAliases( const std::string& address);
		static void				getAliases( const std::string& address, std::vector<std::pair<std::string, std::string> >& aliases);

		static bool	running() 		{ return fRunning; }

		bool	oscDebug() const;
		void	quit()						{ fRunning = false; }
		int		getUDPInPort() const		{ return fUDP.fInPort; }
		int		getUDPOutPort() const		{ return fUDP.fOutPort; }
		int		getUDPErrPort() const		{ return fUDP.fErrPort; }
		const std::string&	getUDPOutAddress() const		{ return fUDP.fOutDstAddress; }
		const std::string&	getUDPErrAddress() const		{ return fUDP.fErrDstAddress; }
		
		virtual void		accept (Updater*);
		virtual void		print(std::ostream& out) const;

		/*!
			\brief creates the object virtual nodes
		*/
		virtual void	createVirtualNodes ();

		/*!
			\brief application processing of a message
			
			Processing a message at application level differs from general scheme since the IAppl object
			call its self \c execute method while this method is called by the parent object when not at the root
			of the model tree.
			\param address the current osc address location
			\param addressTail the remainging part of the osc address
			\param msg the message to be processed
			\return true when the message has been successfully processed i.e. when the model is modified
		*/
		virtual int processMsg (const std::string& address, const std::string& addressTail, const IMessage* msg);
		
		void		setUDPInPort(int p)			{ fUDP.fInPort = p; }
		void		setUDPOutPort(int p)		{ fUDP.fOutPort = p; }
		void		setUDPErrPort(int p)		{ fUDP.fErrPort = p; }

		void		resetBench();
		bool		offscreen()	const			{ return fOffscreen; }
		void		ptask ();

		static std::string checkRootPath (const std::string& path);

	protected:
				 IAppl(int udpport, int outport, int errport,  QApplication* appl, bool offscreen);
		virtual ~IAppl();

		void		setRootPath(const std::string& s);
		void		setUDPOutAddress(const std::string& a)	{ fUDP.fOutDstAddress = a; }
		void		setUDPErrAddress(const std::string& a)	{ fUDP.fErrDstAddress = a; }
		void		setDefaultShow(bool state)				{ fDefaultShow = state; }

		virtual		IMessageList getAll () const;

		IMessage*	hello() const;
		void		helloMsg() const;
//		void		activate() const;
		std::string	guidoversion() const;
		std::string	musicxmlversion() const;

		/// \brief application \c 'require' message handler. Provided to check for version number.
		virtual MsgHandler::msgStatus requireMsg(const IMessage* msg);

		/// \brief application \c 'load' message handler.
		virtual MsgHandler::msgStatus loadMsg (const IMessage* msg);

		/// \brief application \c 'load' message handler.
		virtual MsgHandler::msgStatus cursor (const IMessage* msg);

#ifdef RUNBENCH
		void	startBench()			{ bench::start(); }
		void	stopBench()				{ bench::stop(); }
		MsgHandler::msgStatus	writeBench(const IMessage* msg);
#endif
};

/*! @} */

} // end namespoace

#endif
