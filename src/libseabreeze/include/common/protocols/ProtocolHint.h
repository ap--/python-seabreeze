/***************************************************//**
 * @file    ProtocolHint.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * Hints are used to identify particular characteristics about
 * protocol Transfer objects.  A hint may be used to indicate
 * to a bus some detail it needs about making a transfer, e.g.
 * what endpoint would be appropriate for USB.  Note that the
 * bus (or its helpers) are under no obligation to respect hints.
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

#ifndef SEABREEZE_PROTOCOLHINT_H
#define SEABREEZE_PROTOCOLHINT_H

#include "common/SeaBreeze.h"
#include <string>

namespace seabreeze {

    class ProtocolHint {
    public:
        ProtocolHint(int id, std::string desc);

        /* For derived classes that will fill in their own values
         * and for containers to be able to initialize themselves.
         * This does not set any meaningful values and should not be
         * relied on to create a proper instance.
         */
        ProtocolHint();

        virtual ~ProtocolHint();
        std::string getDescription();
        int getID() const;

        /* Overloading the equality operator so that this can be
         * used as a key for hash_map associations without the actual
         * key objects having to be identical.
         */
        bool operator==(const ProtocolHint &that);

    protected:
        int id;
        std::string description;
    };

}

#endif /* SEABREEZE_PROTOCOLHINT_H */
