/***************************************************//**
 * @file    OBPRequestBufferedSpectrum32AndMetadataExchange.cpp
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
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPRequestBufferedSpectrum32AndMetadataExchange.h"
#include "vendors/OceanOptics/protocols/obp/hints/OBPSpectrumHint.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPMessage.h"
#include "vendors/OceanOptics/protocols/obp/constants/OBPMessageTypes.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPRequestBufferedSpectrum32AndMetadataExchange::OBPRequestBufferedSpectrum32AndMetadataExchange() {
    OBPMessage message;
    vector<unsigned char> *stream;
    unsigned int i;

    this->hints->push_back(new OBPSpectrumHint());

    this->direction = Transfer::TO_DEVICE;

    message.setMessageType(OBPMessageTypes::OBP_GET_BUF_SPEC32_META);
    stream = message.toByteStream();

    this->length = (unsigned) stream->size();
    this->buffer->resize(stream->size());

    for(i = 0; i < stream->size(); i++) {
        (*(this->buffer))[i] = (*stream)[i];
    }
    delete stream;

    checkBufferSize();
}

OBPRequestBufferedSpectrum32AndMetadataExchange::~OBPRequestBufferedSpectrum32AndMetadataExchange() {

}
