// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/chrono.hpp>

using namespace std;

boost::chrono::steady_clock::time_point GetTimepointFromString(const string& timeString, int millisec = 0);

// TODO: reference additional headers your program requires here
