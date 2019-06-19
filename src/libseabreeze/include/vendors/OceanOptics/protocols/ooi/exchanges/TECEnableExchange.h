/***************************************************//**
 * @file    TECEnableExchange.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * This is made abstract so that subclasses specific to
 * the QE/NIRQuest protocol and the old legacy NIR256/512
 * can coexist.  The NIR256/512 are not going to be supported
 * at first, but the design will leave room for them.
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

#ifndef SEABREEZE_TECENABLEEXCHANGE_H
#define SEABREEZE_TECENABLEEXCHANGE_H

#include "common/protocols/Transfer.h"

namespace seabreeze {
  namespace ooiProtocol {
    class TECEnableExchange : public Transfer {
    public:
        TECEnableExchange();
        virtual ~TECEnableExchange();

        virtual void setEnable(bool enable) = 0;
    };
  }
}

#endif
