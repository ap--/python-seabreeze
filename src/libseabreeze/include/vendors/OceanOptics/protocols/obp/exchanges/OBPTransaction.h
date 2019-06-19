/***************************************************//**
 * @file    OBPTransaction.h
 * @date    March 2011
 * @author  Ocean Optics, Inc.
 *
 * All messages in the Ocean Binary Protocol begin with
 * a standard 64-byte header.  It is always safe to read
 * 64 bytes for the start of a new transfer from a device
 * that supports this protocol, which works nicely with
 * the USB minimum packet size.
 *
 * This class simplifies the task of performing transactions
 * with the device, where a command is always followed
 * by a response.
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

#ifndef OBPTRANSACTION_H
#define OBPTRANSACTION_H

#include <vector>
#include "common/buses/TransferHelper.h"
#include "common/protocols/ProtocolHint.h"
#include "common/exceptions/ProtocolException.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPMessage.h"

namespace seabreeze {
    namespace oceanBinaryProtocol {
        class OBPTransaction {
        public:
            OBPTransaction();
            virtual ~OBPTransaction();

            virtual const std::vector<ProtocolHint *> &getHints();

        protected:
            /* This creates a message of the given type and payload and sends it
             * to the device.  The reply is formatted into a byte vector.  Any
             * errors will be indicated via an exception.
             */
            virtual std::vector<byte> *queryDevice(TransferHelper *helper,
                    unsigned int messageType,
                    std::vector<byte> &data) throw (ProtocolException);

            /* This creates a message of the given type and payload and sends it
             * to the device.  No response (other than an acknowledgment) is
             * expected.  This will return true if the command was acknowledged
             * correctly, or false if there was a negative acknowledgment (NACK).  Note
             * that some commands will normally return a NACK even though it was
             * a correct command (e.g. trying to read out a calibration that does
             * not exist) so this does not throw an exception on a NACK.
             */
            virtual bool sendCommandToDevice(TransferHelper *helper,
                    unsigned int messageType,
                    std::vector<byte> &data) throw (ProtocolException);

            std::vector<ProtocolHint *> *hints;
        };
    }
}

#endif /* OBPTRANSACTION_H */
