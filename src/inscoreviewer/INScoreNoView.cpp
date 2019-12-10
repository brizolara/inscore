/*

  Interlude Prototype
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
#include <windows.h>
#define sleep(n)	Sleep(n*1000)
#else
#include <unistd.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <string>

#include <QApplication>
#include <QTimer>

#include "IAppl.h"
#include "INScore.h"
#include "INScoreNoView.h"

using namespace inscore;
using namespace std;

#define kUPDPort		7000
#define kTimeInterval	10			// time task interval in milliseconds

static const char* kPortOption = "--port";


//-----------------------------------------------------------------------
static int intopt (const string& opt, int defaultValue, int n, char **argv)
{
	for (int i = 0; i < n; i++) {
		if (opt == argv[i]) {
			int port = ( i+1 == n) ? 0 : atoi(argv[i+1]);
			return port ? port : defaultValue;
		}
	}
	return defaultValue;
}


#if WIN32
#define sep '\\'
#else
#define sep '/'
#endif

//_______________________________________________________________________
void INScoreAppl::open(const string& file)
{
	size_t pos = file.find_last_of (sep);
	if (pos != string::npos) {
		string path = file.substr(0, pos);
		INScore::MessagePtr msg = INScore::newMessage (krootPath_GetSetMethod);
		INScore::add (msg, path.c_str());
		INScore::postMessage ("/ITL", msg);
	}
	INScore::MessagePtr msg = INScore::newMessage (kload_SetMethod);
	INScore::add (msg, file.c_str());
	INScore::postMessage ("/ITL", msg);
}

//_______________________________________________________________________
void INScoreAppl::timerEvent(QTimerEvent *)
{
cerr << "INScoreAppl::timerEvent" << endl; 
	fGlue->timeTask ();
	if (fGlue->getRate() != fRate) {
		fRate = fGlue->getRate();
		killTimer(fTimerId);
		fTimerId = startTimer(fRate);
	}
}

//_______________________________________________________________________
void INScoreAppl::start (int udpinport, int udpoutport)
{
	setApplicationName("INScoreViewer");

	fGlue = INScore::start (udpinport, udpoutport, udpoutport+1, this);
	fRate = fGlue->getRate();
	fTimerId = startTimer (fRate, Qt::PreciseTimer);
}

//_______________________________________________________________________
void INScoreAppl::run()
{
	do {
		usleep (100);
		fGlue->timeTask();
	} while (IAppl::running());
	return 0;
}


#if defined(WIN32) && !defined(_DEBUG)
# define USEWINMAIN
#endif
//_______________________________________________________________________
#ifdef USEWINMAIN
#include <windows.h>
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdParam, int iCmdShow)
{
	int argc = __argc;
	char **argv = __argv;
#else
int main( int argc, char **argv )
{
#endif
	int ret = 1;
	int udpPort = intopt (kPortOption, kUPDPort, argc, argv);
	INScoreAppl appl(argc, argv);
	appl.start (udpPort, kUPDPort+1);

	for (int i = 1; i < argc; i++) {
		const string arg = argv[i];
		if (arg[0] == '-') {
			if (arg == kPortOption) i++;
		}	
		else appl.open (arg);
	}

//	ret = appl.exec();
	appl.run();
	cout << "Bye" << endl;
	return ret;
}
