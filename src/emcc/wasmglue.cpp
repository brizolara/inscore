/*

  INScore Library
  Copyright (C) 2020  Grame

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

#include <iostream>
#include <string.h>

#include <emscripten.h>
#include <emscripten/bind.h>

#include "wasmglue.h"

using namespace emscripten;
using namespace std;

namespace inscore
{

void testjscall(const char *msg) {
	EM_ASM( {
	  console.log ('testjscall says ' + Module.UTF8ToString($0));
	  document.getElementById(Module.UTF8ToString($0)).style.background = "lightblue";
	}, msg);
}

void usediv(const char *div) {
	EM_ASM( {
	  getScene(Module.UTF8ToString($0)).style.color = "red";
	}, div);
}

int  getdiv(const char *id) {
	int div = EM_ASM_INT( {
		  return getScene(Module.UTF8ToString($0));
		}, id);
	return div;
}

TIntSize divGetSize (const char *div)
{
	int w = EM_ASM_INT( {
		  return getSceneWidth(Module.UTF8ToString($0));
		}, div);
	int h = EM_ASM_INT( {
		  return getSceneHeight(Module.UTF8ToString($0));
		}, div);
	return TIntSize(w, h);
}

void testAdd (int a, int b) {
	int n = EM_ASM_INT( {
		  return testadd($0, $1);
		}, a, b);
cout << "test add: " << a << " + " << b << " => " << n << endl;
}

}