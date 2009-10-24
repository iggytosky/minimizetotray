// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <shlobj.h>

#define _SECURE_ATL 1

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

#include <plugin\npapi.h>
#include <plugin\npfunctions.h>
#include <js\jsapi.h>

#include "DebugLog.h"
