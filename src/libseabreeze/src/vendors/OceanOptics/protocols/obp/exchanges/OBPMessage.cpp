/***************************************************//**
 * @file    OBPMessage.cpp
 * @date    January 2011
 * @author  Ocean Optics, Inc.
 *
 * All messages in the Ocean Binary Protocol begin with
 * a standard 64-byte header.  It is always safe to read
 * 64 bytes for the start of a new transfer from a device
 * that supports this protocol, which works nicely with
 * the USB minimum packet size.
 *
 * This class simplifies the task of getting data into
 * and out of the header.
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
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPMessage.h"

#define OBP_MESSAGE_IMMEDIATE_PAYLOAD_LENGTH 16
#define OBP_MESSAGE_CHECKSUM_LENGTH 16

#define OBP_MESSAGE_FLAGS_RESPONSE      (1 << 0)
#define OBP_MESSAGE_FLAGS_ACK           (1 << 1)
#define OBP_MESSAGE_FLAGS_ACK_REQUESTED (1 << 2)
#define OBP_MESSAGE_FLAGS_NACK          (1 << 3)
#define OBP_MESSAGE_FLAGS_EXCEPTION     (1 << 4)

using namespace seabreeze;
using namespace oceanBinaryProtocol;
using namespace std;

void OBPMessage::setupMessage()
{
    this->header = new vector<unsigned char>(2);
    this->footer = new vector<unsigned char>(4);

    (*this->footer)[0] = 0xC5;
    (*this->footer)[1] = 0xC4;
    (*this->footer)[2] = 0xC3;
    (*this->footer)[3] = 0xC2;

    (*this->header)[0] = 0xC1;
    (*this->header)[1] = 0xC0;

    this->payload = 0;
    this->immediateData = 0;
    this->immediateDataLength = 0;

    this->protocolVersion = 0x1100;
    this->flags = 0;
    this->errorNumber = 0;
    this->messageType = 0;
    this->regarding = 0;

    this->bytesRemaining = 20;  /* checksum + footer */

    this->checksumType = 0;
    this->checksum = new vector<unsigned char>(16);
    for(unsigned int i = 0; i < checksum->size(); i++)
	{
        (*(this->checksum))[i] = 0;
    }

    setPayload(0);
    setImmediateData(0);
}

OBPMessage::OBPMessage()
{
    setupMessage();
}

OBPMessage::~OBPMessage()
{
    if(NULL != this->header)
	{
        delete this->header;
    }
    if(NULL != this->footer)
	{
        delete this->footer;
    }
    if(NULL != this->checksum)
	{
        delete this->checksum;
    }
    if(NULL != this->payload)
	{
        delete this->payload;
    }
    if(NULL != this->immediateData)
	{
        delete this->immediateData;
    }
}

OBPMessage *OBPMessage::parseHeaderFromByteStream(vector<unsigned char> *message)
{
    unsigned int i;
    OBPMessage *retval = new OBPMessage();

    if((*retval->header)[0] != (*message)[0] || (*retval->header)[1] != (*message)[1])
	{
        string errorMessage("Could not find message header");
        throw IllegalArgumentException(errorMessage);
    }

    retval->protocolVersion = ((*message)[2] & 0x00FF)
                           | (((*message)[3] & 0x00FF) << 8);
    retval->flags = ((*message)[4] & 0x00FF)
                 | (((*message)[5] & 0x00FF) << 8);
    retval->errorNumber = ((*message)[6] & 0x00FF)
                       | (((*message)[7] & 0x00FF) << 8);
    retval->messageType = ((*message)[8] & 0x00FF)
                       | (((*message)[9] & 0x00FF) << 8)
                       | (((*message)[10] & 0x00FF) << 16)
                       | (((*message)[11] & 0x00FF) << 24);
    retval->regarding = ((*message)[12] & 0x00FF)
                     | (((*message)[13] & 0x00FF) << 8)
                     | (((*message)[14] & 0x00FF) << 16)
                     | (((*message)[15] & 0x00FF) << 24);
    retval->checksumType = (*message)[22];
    retval->immediateDataLength = (*message)[23];
    if(retval->immediateDataLength > 0)
	{
        retval->immediateData = new vector<unsigned char>(retval->immediateDataLength);
        for(i = 0; i < retval->immediateDataLength; i++)
		{
            (*(retval->immediateData))[i] = (*message)[i + 24];
        }
    }
    retval->bytesRemaining = ((*message)[40] & 0x00FF)
                          | (((*message)[41] & 0x00FF) << 8)
                          | (((*message)[42] & 0x00FF) << 16)
                          | (((*message)[43] & 0x00FF) << 24);
    if(retval->bytesRemaining <
            retval->checksum->size() + retval->footer->size())
	{
        string errorMessage("Invalid bytes remaining field");
        throw IllegalArgumentException(errorMessage);
    }

    return retval;
}

OBPMessage *OBPMessage::parseByteStream(vector<unsigned char> *message)
{
    int temp;
    int offset;
    unsigned int i;

    OBPMessage *retval = parseHeaderFromByteStream(message);

    /* Compute the payload length.  This may be zero. */
    temp = retval->bytesRemaining - (unsigned) retval->checksum->size() - (unsigned) retval->footer->size();
    offset = 44;
    if(temp > 0)
	{
        retval->payload = new vector<unsigned char>(temp);
        for(i = 0; i < (unsigned)temp; i++)
		{
			if((*message).size() >= i)
				(*(retval->payload))[i] = (*message)[offset++];
			else
			{
				string errorMessage("OBP Message Error: Could not parse message. Bytes remaining did not match message size.");
				throw IllegalArgumentException(errorMessage);
			}
        }
        /* FIXME: should this delete immediateData too? */
    }
    for(i = 0; i < retval->checksum->size(); i++)
	{
        (*(retval->checksum))[i] = (*message)[offset++];
    }
    for(i = 0; i < retval->footer->size(); i++)
	{
        if((*(retval->footer))[i] != (*message)[offset++])
		{
            string errorMessage("Could not find message footer");
            throw IllegalArgumentException(errorMessage);
        }
    }
    return retval;
}

vector<unsigned char> *OBPMessage::toByteStream()
{
    vector<unsigned char> *retval = new vector<unsigned char>;
    int length = 64;
    unsigned int i;
    int offset = 0;
    int immediateCopied = 0;

    if(0 != this->payload)
	{
        length += (unsigned) this->payload->size();
    }

    retval->resize(length);
    for(i = 0; i < header->size(); i++)
	{
        (*retval)[offset++] = (*this->header)[i];
    }

    (*retval)[offset++] = this->protocolVersion & 0x00FF;
    (*retval)[offset++] = (this->protocolVersion >> 8) & 0x00FF;

    (*retval)[offset++] = this->flags & 0x00FF;
    (*retval)[offset++] = (this->flags >> 8) & 0x00FF;

    (*retval)[offset++] = this->errorNumber & 0x00FF;
    (*retval)[offset++] = (this->errorNumber >> 8) & 0x00FF;

    (*retval)[offset++] = this->messageType & 0x00FF;
    (*retval)[offset++] = (this->messageType >> 8) & 0x00FF;
    (*retval)[offset++] = (this->messageType >> 16) & 0x00FF;
    (*retval)[offset++] = (this->messageType >> 24) & 0x00FF;

    (*retval)[offset++] = this->regarding & 0x00FF;
    (*retval)[offset++] = (this->regarding >> 8) & 0x00FF;
    (*retval)[offset++] = (this->regarding >> 16) & 0x00FF;
    (*retval)[offset++] = (this->regarding >> 24) & 0x00FF;

    for(i = 0; i < 6; i++)
	{
        (*retval)[offset++] = 0;  /* Reserved bytes */
    }

    (*retval)[offset++] = checksumType;
    (*retval)[offset++] = immediateDataLength;
    if(NULL != this->immediateData)
	{
        for(i = 0; i < 16 && i < this->immediateData->size(); i++)
		{
            (*retval)[offset++] = (*(this->immediateData))[i];
            immediateCopied++;
        }
    }

    /* Pad out the rest of the immediateData field in case the length
     * of the vector being copied there was less than the full field length
     * or immediateData was null.
     */
    for(i = immediateCopied; i < 16; i++)
	{
        (*retval)[offset++] = 0;
    }

    (*retval)[offset++] = this->bytesRemaining & 0x00FF;
    (*retval)[offset++] = (this->bytesRemaining >> 8) & 0x00FF;
    (*retval)[offset++] = (this->bytesRemaining >> 16) & 0x00FF;
    (*retval)[offset++] = (this->bytesRemaining >> 24) & 0x00FF;

    if(NULL != this->payload)
	{
        for(i = 0; i < this->payload->size(); i++)
		{
            (*retval)[offset++] = (*this->payload)[i];
        }
    }
    for(i = 0; i < 16; i++) {
        (*retval)[offset++] = 0;   /* Checksum (zero for now) */
    }
    for(i = 0; i < this->footer->size(); i++)
	{
        (*retval)[offset++] = (*this->footer)[i];
    }
    return retval;
}

vector<unsigned char> *OBPMessage::getData()
{
    if(0 != this->immediateData && 0 != this->immediateDataLength)
	{
        return this->immediateData;
    } else if(0 != this->payload && (checksum->size() + footer->size())
            < bytesRemaining)
	{
        return this->payload;
    }
	else
	{
        return new vector<unsigned char>();
    }
}


unsigned int OBPMessage::getBytesRemaining()
{
    return this->bytesRemaining;
}


unsigned char OBPMessage::getChecksumType()
{
    return this->checksumType;
}


unsigned short OBPMessage::getErrno()
{
    return this->errorNumber;
}


unsigned short OBPMessage::getFlags()
{
    return this->flags;
}


vector<unsigned char> *OBPMessage::getImmediateData()
{
    return this->immediateData;
}


unsigned char OBPMessage::getImmediateDataLength()
{
    return this->immediateDataLength;
}


unsigned int OBPMessage::getMessageType()
{
    return this->messageType;
}


vector<unsigned char> *OBPMessage::getPayload()
{
    return this->payload;
}


unsigned short OBPMessage::getProtocolVersion()
{
    return this->protocolVersion;
}


unsigned int OBPMessage::getRegarding()
{
    return this->regarding;
}


bool OBPMessage::isAckFlagSet()
{
    return (0 == (this->flags & OBP_MESSAGE_FLAGS_ACK)) ? false : true;
}

bool OBPMessage::isNackFlagSet()
{
    return (0 == (this->flags & OBP_MESSAGE_FLAGS_NACK)) ? false : true;
}

void OBPMessage::setAckRequestedFlag()
{
    this->flags |= OBP_MESSAGE_FLAGS_ACK_REQUESTED;
}

void OBPMessage::setBytesRemaining(unsigned int remaining)
{
    this->bytesRemaining = remaining;
}


void OBPMessage::setChecksumType(unsigned char t)
{
    this->checksumType = t;
}


void OBPMessage::setData(vector<unsigned char> *data)
{
    if(NULL == data || data->size() <= 16)
	{
        setImmediateData(data);
        setPayload(NULL);  /* Just in case data == NULL */
    }
	else
	{
        setPayload(data);
    }
}

void OBPMessage::setErrorNumber(unsigned short errNumber)
{
    this->errorNumber = errNumber;
}


void OBPMessage::setFlags(unsigned short f)
{
    this->flags = f;
}


void OBPMessage::setImmediateData(vector<unsigned char> *data)
{
    if(0 != this->immediateData)
	{
        delete this->immediateData;
    }
    this->immediateData = data;
    if(0 != this->immediateData)
	{
        if(this->immediateData->size() <= 16)
		{
            this->immediateDataLength = (unsigned char)this->immediateData->size();
        }
		else
		{
            this->immediateDataLength = 16;
        }
        /* Payload and immediate data are mutually exclusive */
        setPayload(NULL);
    }
	else
	{
        this->immediateDataLength = 0;
    }
}


void OBPMessage::setImmediateDataLength(unsigned char len)
{
    this->immediateDataLength = len;
}


void OBPMessage::setMessageType(unsigned int t)
{
    this->messageType = t;
}


void OBPMessage::setPayload(vector<unsigned char> *data)
{
    if(NULL != this->payload)
	{
        delete this->payload;
    }
    this->payload = data;
    if(NULL != this->payload)
	{
        this->bytesRemaining = (unsigned) (this->payload->size() + this->checksum->size()
            + this->footer->size());
        if(payload->size() > 0)
		{
            /* Payload and immediate data are mutually exclusive */
            setImmediateData(NULL);
        }
    }
	else
	{
        this->bytesRemaining = (unsigned) (this->checksum->size() + this->footer->size());
    }
}


void OBPMessage::setProtocolVersion(unsigned short version)
{
    this->protocolVersion = version;
}


void OBPMessage::setRegarding(unsigned int r)
{
    this->regarding = r;
}
