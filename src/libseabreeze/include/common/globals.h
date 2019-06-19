/**
* @file     globals.h
* @author   Mark Zieg <mark.zieg@oceanoptics.com>
* @date     Sep 26, 2012
* @brief    Provides a single point of maintenance for anything you want
*           included, defined, or set across every file in the application
*           (such as memory profiling, etc).  Normally empty.
*
* LICENSE:
*
* SeaBreeze Copyright (C) 2014, Ocean Optics Inc
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject
* to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef SEABREEZE_GLOBALS_H
#define SEABREEZE_GLOBALS_H

// change to "#if 1" to enable memory heap debugging under Visual Studio
#if 0
    #ifdef _WINDOWS

        // For these to work right, you need to #define _CRTDBG_MAP_ALLOC and
        // _CRTDBG_MAP_ALLOC_NEW in your Visual Studio project (i.e., Project
        // -> Configuration Properties -> C/C++ -> Preprocessor -> Preprocessor
        // Definitions).  #Defining them here in your header files DOES NOT
        // WORK, because Visual Studio will internally include many system
        // headers (including stdafx.h) long before you get here.
        //
        // @see http://msdn.microsoft.com/en-us/library/e5ewb1h3%28v=vs.80%29.aspx
        // @see http://social.msdn.microsoft.com/Forums/en/vcgeneral/thread/ebc7dd7a-f3c6-49f1-8a60-e381052f21b6,

        #pragma message("  (Windows memory debugging enabled)")

        // these will provide leak profiling for C malloc(), etc
        #include <stdlib.h>
        #include <crtdbg.h>

        // these will provide leak profiling for C++ 'new'
        #ifndef DBG_NEW
            #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
            #define new DBG_NEW
        #endif

    #endif
#endif

#endif
