/**
    @file   Log.cpp
    @brief  Implementation of Log
    @author Mark Zieg

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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "common/Log.h"

using std::stack;
using std::string;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                               C++ Interface                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

unsigned Log::logLevel = OOI_LOG_LEVEL_NEVER;
FILE* Log::logFile  = stdout;
stack<string>* Log::callstack = NULL;

Log::Log(const char *func)
{
#ifdef OOI_DEBUG
    if (NULL == callstack)
        callstack = new stack<string>();
    callstack->push(string(func));
    trace("[entering]");
#endif
}

Log::~Log()
{
#ifdef OOI_DEBUG
    trace("[returning]");
    callstack->pop();
#endif
}

void Log::setLogLevel(int lvl)
{
    logLevel = (unsigned) lvl;
}

void Log::setLogLevel(const string& s)
{
    string t(s);
    for (unsigned i = 0; i < t.size(); i++)
        if ('A' <= t[i] && t[i] <= 'Z')
            t[i] += 'a' - 'A';

         if (t == "never") setLogLevel(OOI_LOG_LEVEL_NEVER);
    else if (t == "error") setLogLevel(OOI_LOG_LEVEL_ERROR);
    else if (t == "warn" ) setLogLevel(OOI_LOG_LEVEL_WARN);
    else if (t == "info" ) setLogLevel(OOI_LOG_LEVEL_INFO);
    else if (t == "debug") setLogLevel(OOI_LOG_LEVEL_DEBUG);
    else if (t == "trace") setLogLevel(OOI_LOG_LEVEL_TRACE);
}

void Log::setLogFile(void *f)
{
    if (logFile != NULL)
        fflush(logFile);
    logFile = (FILE*) f;
    if (logFile != NULL)
        fflush(logFile);
}

void Log::trace(const char *fmt, ...)
{
#ifdef OOI_DEBUG
	va_list args;
    if(logLevel < OOI_LOG_LEVEL_TRACE)
        return;
    va_start(args, fmt);
    formatAndSend(OOI_LOG_LEVEL_TRACE, "TRACE", ":", fmt, args);
    va_end(args);
#endif
}

void Log::debug(const char *fmt, ...)
{
#ifdef OOI_DEBUG
	va_list args;
    if(logLevel < OOI_LOG_LEVEL_DEBUG)
        return;
    va_start(args, fmt);
    formatAndSend(OOI_LOG_LEVEL_DEBUG, "DEBUG", ":", fmt, args);
    va_end(args);
#endif
}

void Log::info(const char *fmt, ...)
{
#ifdef OOI_DEBUG
    va_list args;
    if(logLevel < OOI_LOG_LEVEL_INFO)
        return;
    va_start(args, fmt);
    formatAndSend(OOI_LOG_LEVEL_INFO, "INFO", ":", fmt, args);
    va_end(args);
#endif
}

void Log::warn(const char *fmt, ...)
{
#ifdef OOI_DEBUG
    va_list args;
    if(logLevel < OOI_LOG_LEVEL_WARN)
        return;
    va_start(args, fmt);
    formatAndSend(OOI_LOG_LEVEL_WARN, "WARN", ">>>", fmt, args);
    va_end(args);
#endif
}

void Log::error(const char *fmt, ...)
{
#ifdef OOI_DEBUG
    va_list args;
    if(logLevel < OOI_LOG_LEVEL_ERROR)
        return;
    va_start(args, fmt);
    formatAndSend(OOI_LOG_LEVEL_ERROR, "ERROR", "***", fmt, args);
    va_end(args);
#endif
}

void Log::formatAndSend(
    int lvl,
    const char *lvlName,
    const char *separator,
    const char *fmt,
    va_list args)
{
    if (logFile == NULL) {
        return;
    }

	unsigned indent = (unsigned int) (callstack->size() - 1) * 4;
	if (OOI_LOG_LEVEL_TRACE == lvl && indent > 2) {
		indent -= 2;
	}

    fprintf(logFile, "seabreeze %-7s%-3s%*s%s: ",
        lvlName,
        separator,
        indent,
        "",
        callstack->top().c_str()
    );
    fflush(logFile);

    vfprintf(logFile, fmt, args);
    if (fmt[strlen(fmt)] != '\n') {
        fprintf(logFile, "\n");
    }
	fflush(logFile);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                               C Interface                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void seabreeze_log_set_level_int(int lvl)
{
    Log::setLogLevel(lvl);
}

void seabreeze_log_set_level_string(const char *s)
{
    Log::setLogLevel(s);
}


void seabreeze_log_debug(const char *fmt, ...)
{
#ifdef OOI_DEBUG
    Log logger("");
	va_list args;
    if(logger.logLevel < OOI_LOG_LEVEL_DEBUG)
        return;
    va_start(args, fmt);
    logger.formatAndSend(OOI_LOG_LEVEL_DEBUG, "DEBUG", ":", fmt, args);
    va_end(args);
#endif
}

void seabreeze_log_info (const char *fmt, ...)
{
#ifdef OOI_DEBUG
    Log logger("");
    va_list args;
    if(logger.logLevel < OOI_LOG_LEVEL_INFO)
        return;
    va_start(args, fmt);
    logger.formatAndSend(OOI_LOG_LEVEL_INFO, "INFO", ":", fmt, args);
    va_end(args);
#endif
}

void seabreeze_log_warn (const char *fmt, ...)
{
#ifdef OOI_DEBUG
    Log logger("");
    va_list args;
    if(logger.logLevel < OOI_LOG_LEVEL_WARN)
        return;
    va_start(args, fmt);
    logger.formatAndSend(OOI_LOG_LEVEL_WARN, "WARN", ">>>", fmt, args);
    va_end(args);
#endif
}

void seabreeze_log_error(const char *fmt, ...)
{
#ifdef OOI_DEBUG
    Log logger("");
    va_list args;
    if(logger.logLevel < OOI_LOG_LEVEL_ERROR)
        return;
    va_start(args, fmt);
    logger.formatAndSend(OOI_LOG_LEVEL_ERROR, "ERROR", "***", fmt, args);
    va_end(args);
#endif
}
