/***************************************************//**
 * @file    Exchange.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * This is simply an interface that other classes will
 * extend to have a common transfer() and getHints methods
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

#ifndef SEABREEZE_EXCHANGE_H
#define SEABREEZE_EXCHANGE_H

#include <vector>
#include "common/buses/TransferHelper.h"
#include "common/protocols/ProtocolHint.h"
#include "common/Data.h"
#include "common/exceptions/ProtocolException.h"

namespace seabreeze {

    class Exchange {
    public:
        Exchange();
        Exchange(std::vector<ProtocolHint *> *hints);
        virtual ~Exchange();
        virtual Data *transfer(TransferHelper *helper) throw (ProtocolException) = 0;
        virtual const std::vector<ProtocolHint *> &getHints();

    protected:
        std::vector<ProtocolHint *> *hints;
    };

}

#endif /* SEABREEZE_EXCHANGE_H */

