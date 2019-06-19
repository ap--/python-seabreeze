/***************************************************//**
 * @file    ProtocolHelper.h
 * @date    July 2009
 * @author  Ocean Optics, Inc.
 *
 * Feature instances may look up an implementation object
 * that matches a particular Protocol.  All such implementations
 * should in some way derive from ProtocolHelper so that
 * Feature's look up mechanism can return them.  It is
 * expected that each Feature will have a corresponding
 * interface at the Protocol layer; those interface classes
 * should derive from this class, and their implementations
 * will thus extend this as well.
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
 *******************************************************/

#ifndef SEABREEZE_PROTOCOLHELPER_H
#define SEABREEZE_PROTOCOLHELPER_H

#include "common/protocols/Protocol.h"

namespace seabreeze {

    class ProtocolHelper {
    public:
        ProtocolHelper(Protocol *proto);
        virtual ~ProtocolHelper();
        Protocol &getProtocol();

    protected:
        /* Protected for derived classes to use. */
        ProtocolHelper();
        Protocol *protocol;
    };

}

#endif
