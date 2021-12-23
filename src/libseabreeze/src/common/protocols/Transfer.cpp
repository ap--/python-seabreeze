/***************************************************//**
 * @file    Transfer.cpp
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
#include "common/protocols/Transfer.h"
#include "common/ByteVector.h"
#include <string>

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

using namespace seabreeze;
using namespace std;

const direction_t Transfer::TO_DEVICE = 1;
const direction_t Transfer::FROM_DEVICE = 2;

Transfer::Transfer(vector<ProtocolHint *> *hints, vector<unsigned char> *buffer,
        direction_t direction, unsigned int length) : Exchange(hints) {
    this->buffer = buffer;
    this->direction = direction;
    this->length = length;

    checkBufferSize();
}

/* Protected default constructor; assumes that subclass will fill
 * in the values required by the public constructor.  Since these are
 * complex types which probably have to be declared and populated before
 * they can be used, they are not really suitable for the initializer list.
 */
Transfer::Transfer() {
    this->buffer = new vector<unsigned char>;
    this->length = 0;

    checkBufferSize();
}

Transfer::~Transfer() {
    delete this->buffer;
}

Data *Transfer::transfer(TransferHelper *helper) {
    int flag = 0;

    /* Execute the actual movement of the data in this object's buffer
     * across the bus represented by the given TransferHelper.
     */
    if(Transfer::TO_DEVICE == this->direction) {
        try {
            flag = helper->send(*(this->buffer), this->length);
            if(((unsigned int)flag) != this->length) {
                /* FIXME: retry, throw exception, something here */
            }
        } catch (BusException &be) {
            string error("Failed to write to bus.");
            /* FIXME: previous exception should probably be bundled up into the new exception */
            /* FIXME: there is probably a more descriptive type for this than ProtocolException */
            throw ProtocolException(error);
        }
        return NULL;
    } else if(Transfer::FROM_DEVICE == this->direction) {
        try {
            flag = helper->receive(*(this->buffer), this->length);
            if(((unsigned int)flag) != this->length) {
                /* FIXME: retry, throw exception, something here */
            }
        } catch (BusException &be) {
            string error("Failed to write to bus.");
            /* FIXME: previous exception should probably be bundled up into the new exception */
            /* FIXME: there is probably a more descriptive type for this than ProtocolException */
            throw ProtocolException(error);
        }

        /* A copy is made of the data before it is sent out for two
         * reasons.  First, this provides safety from the recipient
         * trying to delete it.  Second, it will make it easier for this
         * to be thread-safe later (though anything that touches the
         * buffer internal to this instance will need to be synchronized
         * with other accesses, especially where a derived class calls this
         * method then expects the buffer to be filled in with
         * something useful).  Yes, this incurs overhead, but not much.
         */
        ByteVector *retval = new ByteVector(*(this->buffer));
        return retval;
    } else {
        string error("Invalid transfer direction specified.");
        /* FIXME: there is probably a more descriptive type for this than ProtocolException */
        throw ProtocolException(error);
    }
    return NULL;
}

void Transfer::checkBufferSize() {
    if(this->buffer->size() < this->length) {
        this->buffer->resize(this->length);
    }
}
