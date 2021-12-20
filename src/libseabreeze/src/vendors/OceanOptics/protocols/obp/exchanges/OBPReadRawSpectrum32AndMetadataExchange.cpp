/***************************************************//**
 * @file    OBPReadRawSpectrum32AndMetadataExchange.cpp
 * @date    September 2013
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
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPReadRawSpectrum32AndMetadataExchange.h"
#include "vendors/OceanOptics/protocols/obp/hints/OBPSpectrumHint.h"
#include "vendors/OceanOptics/protocols/obp/constants/OBPMessageTypes.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPMessage.h"
#include "common/ByteVector.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

#define METADATA_LENGTH         32
#define OBP_MESSAGE_OVERHEAD    64

OBPReadRawSpectrum32AndMetadataExchange::OBPReadRawSpectrum32AndMetadataExchange(
        unsigned int pixels) {

    this->hints->push_back(new OBPSpectrumHint());
    this->direction = Transfer::FROM_DEVICE;

    this->metadataLength = METADATA_LENGTH;
    setNumberOfPixels(pixels);
}

OBPReadRawSpectrum32AndMetadataExchange::~OBPReadRawSpectrum32AndMetadataExchange() {

}

void OBPReadRawSpectrum32AndMetadataExchange::setNumberOfPixels(int pixels) {
    unsigned int readoutLength;

    this->numberOfPixels = pixels;
    readoutLength = (this->numberOfPixels * 4) + OBP_MESSAGE_OVERHEAD + METADATA_LENGTH;

    this->buffer->resize(readoutLength);
    this->length = readoutLength;
    checkBufferSize();
}

unsigned int OBPReadRawSpectrum32AndMetadataExchange::isLegalMessageType(unsigned int t) {
    if(OBPMessageTypes::OBP_GET_BUF_SPEC32_META == t) {
        return 1;
    }

    /* More message types can be added as legal if required, or this function
     * can be overridden so that subclasses can specify more exactly what kind
     * of message they are meant to decode.
     */

    return 0;
}

Data *OBPReadRawSpectrum32AndMetadataExchange::transfer(TransferHelper *helper) {
    Data *xfer;
    OBPMessage *message = NULL;
    vector<unsigned char> *bytes;

    /* This will use the superclass to transfer data from the device
     */
    xfer = Transfer::transfer(helper);
    if(NULL == xfer) {
        string error("Expected Transfer::transfer to produce a non-null result "
                        "containing raw spectral data.  Without this data, it is not "
                        "possible to generate a valid formatted spectrum.");
        throw ProtocolException(error);
    }
    /* xfer is a copy of what is already stored in this->buffer so delete it.
     */
    delete xfer;

    /* Try to parse the buffer into an OBPMessage.  This may throw an exception
     * if the message is badly formed.
     */
    try {
        message = OBPMessage::parseByteStream(this->buffer);
    } catch (IllegalArgumentException &iae) {
        string error("Failed to parse message transferred from device");
        throw ProtocolException(error);
    }

    if(0 == isLegalMessageType(message->getMessageType())) {
        string error("Did not get expected message type, got ");
        error += message->getMessageType();
        throw ProtocolException(error);
    }

    /* Extract the pixel data from the message.  Note that this is stripping
     * off the metadata.  It might be desirable to stuff the metadata into the
     * resulting Data instance for later use.
     */
    bytes = message->getData();
    if(bytes->size() < (this->numberOfPixels * 4) + METADATA_LENGTH) {
        string error("Spectrum response does not have enough data.");
        delete message;
        throw ProtocolException(error);
    }
    /* This incurs a copy of the buffer so it is safe to delete the message. */
    ByteVector *retval = new ByteVector(*bytes);
    delete message;

    return retval;
}
