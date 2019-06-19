/***************************************************//**
 * @file    ProtocolFamilies.h
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This provides a way to describe different kinds
 * protocols (e.g. OOI, OBP) generically.
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

#ifndef SEABREEZE_PROTOCOLFAMILIES_H
#define SEABREEZE_PROTOCOLFAMILIES_H

#include "common/protocols/ProtocolFamily.h"
#include <vector>

namespace seabreeze {
    namespace api {

        class UndefinedProtocolFamily : public ProtocolFamily {
        public:
            UndefinedProtocolFamily();
            virtual ~UndefinedProtocolFamily();
        };

        class OOIProtocolFamily : public ProtocolFamily {
        public:
            OOIProtocolFamily();
            virtual ~OOIProtocolFamily();
        };

        class OceanBinaryProtocolFamily : public ProtocolFamily {
        public:
            OceanBinaryProtocolFamily();
            virtual ~OceanBinaryProtocolFamily();
        };

        class JazMessagingProtocolFamily : public ProtocolFamily {
        public:
            JazMessagingProtocolFamily();
            virtual ~JazMessagingProtocolFamily();
        };

        class VirtualProtocolFamily : public ProtocolFamily {
        public:
            VirtualProtocolFamily();
            virtual ~VirtualProtocolFamily();
        };

        class ProtocolFamilies {
        public:
            const UndefinedProtocolFamily UNDEFINED_PROTOCOL;
            const OOIProtocolFamily OOI_PROTOCOL;
            const OceanBinaryProtocolFamily OCEAN_BINARY_PROTOCOL;
            const JazMessagingProtocolFamily JAZ_MESSAGING_PROTOCOL;
            const VirtualProtocolFamily VIRTUAL_PROTOCOL;

            ProtocolFamilies();
            ~ProtocolFamilies();
            std::vector<ProtocolFamily *> getAllProtocolFamilies();
        };
    }
}

#endif
