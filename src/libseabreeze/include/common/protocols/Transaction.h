/***************************************************//**
 * @file    Transaction.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * The Transaction class is simply a wrapper
 * around one or more Transfer objects that must
 * be executed in a particular order.  This is
 * provided for convenience.  Some transfers
 * to Ocean Optics spectrometers put the spectrometer
 * into a particular state where it expects another
 * action to be taken, and Transaction objects can
 * be used to ensure that all expected operations occur.
 *
 * Some actions, like requesting a spectrum, do not
 * necessarily require that the next action be a read
 * operation.  Thus, some Transfers that appear to follow
 * a causal chain may not in fact make good Transactions.
 * In this case, reading the status of the device until
 * it reports data ready is a common operation before
 * reading the spectrum.
 *
 * Note that the Transaction class has no notion of buses
 * or protocols, and this is by design.
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

#ifndef SEABREEZE_TRANSACTION_H
#define SEABREEZE_TRANSACTION_H

#include <vector>
#include "common/protocols/Exchange.h"
#include "common/protocols/Transfer.h"
#include "common/buses/TransferHelper.h"
#include "common/protocols/ProtocolHint.h"
#include "common/Data.h"

namespace seabreeze {

    class Transaction : public Exchange {
    public:
        Transaction();
        virtual ~Transaction();
        void addTransfer(Transfer *xfer);

        /* Inherited from Exchange */
        virtual Data *transfer(TransferHelper *helper) throw (ProtocolException);

    protected:
        std::vector<Transfer *> transfers;

    private:
        void updateHints();

    };

}

#endif /* SEABREEZE_TRANSACTION_H */

