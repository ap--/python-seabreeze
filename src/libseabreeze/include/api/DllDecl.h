/***************************************************//**
 * @file    DllDecl.h
 * @date    September 2012
 * @author  Ocean Optics, Inc.
 *
 * This class provides the DLL import/export decorations
 * required for Windows DLLs.  It is encapsulated to
 * reduce copy-paste and to allow a single point of
 * maintenance.
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
 ******************************************************/

#ifndef SEABREEZE_DLL_DECL_H
#define SEABREEZE_DLL_DECL_H

#ifdef _WINDOWS
    #ifdef BUILD_DLL
        #define DLL_DECL __declspec(dllexport)
    #else
        #define DLL_DECL __declspec(dllimport)
    #endif

    // "STL member 'seabreeze::api::Foo::foo' needs to have dll-interface
    // to be used by clients of class 'seabreeze::api::Foo'"
    #pragma warning (disable: 4251)

    // "non dll-interface class 'seabreeze::Foo' used as base for dll-interface
    // class 'seabreeze::api::Bar'"
    #pragma warning (disable: 4275)
#else
    #define DLL_DECL
#endif

#endif
