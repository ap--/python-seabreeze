/**
    @file   Log.h
    @brief  Interface to Log
    @author Mark Zieg <mark.zieg@oceanoptics.com>

    LICENSE:

    SeaBreeze Copyright (C) 2014, Ocean Optics Inc

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject
    to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef SEABREEZE_LOG_H
#define SEABREEZE_LOG_H

// #include "api/DllDecl.h"

#include <string>
#include <stack>
#include <stdio.h>
#include <stdarg.h>

#ifdef OOI_DEBUG
    #define OOI_LOG_PRINT 1
#else
    #define OOI_LOG_PRINT 0
#endif

#ifdef __cplusplus

/**
* @brief instantiate logger in the current function
* @param s (Input) function name (typically __FUNCTION__)
*/
#define LOG(s) Log logger(s);

/**
* @brief log a printf string (and optional arguments) if debugging is enabled
* @note double parens: call as LOG_DEBUG(("variable x is %d, y is %f", x, y));
* @see http://stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing
*/
#define LOG_DEBUG(s) do { if (OOI_LOG_PRINT) logger.debug s; } while (0)

//! @see LOG_DEBUG
#define LOG_INFO(s)  do { if (OOI_LOG_PRINT) logger.info  s; } while (0)

//! @see LOG_DEBUG
#define LOG_WARN(s)  do { if (OOI_LOG_PRINT) logger.warn  s; } while (0)

//! @see LOG_DEBUG
#define LOG_ERROR(s) do { if (OOI_LOG_PRINT) logger.error s; } while (0)

#define OOI_LOG_LEVEL_NEVER 0
#define OOI_LOG_LEVEL_ERROR 1
#define OOI_LOG_LEVEL_WARN  2
#define OOI_LOG_LEVEL_INFO  3
#define OOI_LOG_LEVEL_DEBUG 4
#define OOI_LOG_LEVEL_TRACE 5

/**
* @brief Simple logger for OOI applications.
* @todo  Provide better thread support (hard to tell what thread model
*        the caller may be using...)
* @todo  Provide flat C interface (e.g. for NativeUSBWinUSB.c, test apps)
*
* Provides automatic heirarchical call-stack indentation.
*/
class Log
{
    public:
        Log(const char *s);
       ~Log();

        // public class methods
        static void setLogLevel(int lvl);
        static void setLogLevel(const std::string& s);
        static void setLogFile(void *f);

        // public instance methods
        void debug(const char *fmt, ...);
        void info (const char *fmt, ...);
        void warn (const char *fmt, ...);
        void error(const char *fmt, ...);

        // these must be public for C interface to work
        static unsigned logLevel;
        void formatAndSend(int lvl, const char *lvlName,
            const char *separator, const char *fmt, va_list args);

    private:
        // private class attributes
        static FILE *logFile;
        static std::stack<std::string>* callstack;

        // private instance methods
        void trace(const char *fmt, ...);
};

// extern "C" {
#endif /* __cplusplus */

#if 0
// We need the flattened C interface if we want to call from Cygwin (mainly
// to set log level)...see http://cygwin.com/ml/cygwin/2006-04/msg00251.html
void DLL_DECL seabreeze_log_set_level_int(int lvl);
void DLL_DECL seabreeze_log_set_level_string(const char *s);
void DLL_DECL seabreeze_log_debug(const char *fmt, ...);
void DLL_DECL seabreeze_log_info (const char *fmt, ...);
void DLL_DECL seabreeze_log_warn (const char *fmt, ...);
void DLL_DECL seabreeze_log_error(const char *fmt, ...);

#endif

#ifdef __cplusplus
// };

#endif /* __cplusplus */
#endif
