/***************************************************//**
 * @file    Transaction.cpp
 * @date    February 2009
 * @author  Ocean Optics, Inc.
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

#include "common/globals.h"
#include "common/protocols/Transaction.h"

using namespace seabreeze;
using namespace std;

Transaction::Transaction() {

}

Transaction::~Transaction() {
    /* This class does not own any of the hints.  Their lifecycle is
     * maintained by the Transfers that created them.  This simply
     * stops tracking the hints so that the Exchange destructor does
     * not try to destroy them.
     *
     * Alternately, they could have been cloned, but that would have
     * added a lot more overhead.
     */
    this->hints->resize(0);

    vector<Transfer *>::iterator iter;
    for(iter = this->transfers.begin(); iter != this->transfers.end(); iter++) {
        delete (*iter);
    }
}

void Transaction::addTransfer(Transfer *xfer) {
    /* Append the transfer to the current queue of transfers. */
    this->transfers.push_back(xfer);

    updateHints();
}

Data *Transaction::transfer(TransferHelper *helper) {
    Data *retval = NULL;
    vector<Transfer *>::iterator iter = this->transfers.begin();
    /* Iterate over all stored transfers and delegate to the helper to
     * move the data.
     */
    for(iter = this->transfers.begin(); iter != transfers.end(); iter++) {
        if(NULL != retval) {
            /* Get rid of any objects returned from any but the last transfer. */
            delete retval;
            retval = NULL;
        }
        /* Note that this may throw a ProtocolException which will not be caught here. */
        retval = (*iter)->transfer(helper);
    }
    /* This has the effect of returning the result of the last transfer. */
    return retval;
}

void Transaction::updateHints() {
    vector<Transfer *>::iterator iter;
    vector<ProtocolHint *>::iterator hintIter;

    /* This class does not own any of the hints.  Their lifecycle is
     * maintained by the Transfers that created them.  This simply
     * stops tracking the hints so that the vector can be repopulated.
     */
    this->hints->resize(0);

    /* Iterate over all stored transfers and gather up their hints.
     */
    for(iter = this->transfers.begin(); iter != this->transfers.end(); iter++) {
        vector<ProtocolHint *> h = (*iter)->getHints();
        for(hintIter = h.begin(); hintIter != h.end(); hintIter++) {
            this->hints->push_back(*hintIter);
        }
    }
}
