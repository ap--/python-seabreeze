/***************************************************//**
 * @file    FastBufferFeatureAdapter.cpp
 * @date    February 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze DataBufferFeatureInterface instances.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2017, Ocean Optics Inc
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
#include "api/seabreezeapi/FastBufferFeatureAdapter.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"

using namespace seabreeze;
using namespace seabreeze::api;

FastBufferFeatureAdapter::FastBufferFeatureAdapter(
    FastBufferFeatureInterface *intf, const FeatureFamily &f,
        Protocol *p, Bus *b, unsigned short instanceIndex)
    : FeatureAdapterTemplate<FastBufferFeatureInterface>(intf, f, p, b, instanceIndex)
{

}

FastBufferFeatureAdapter::~FastBufferFeatureAdapter()
{

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

unsigned char FastBufferFeatureAdapter::getBufferingEnable(int *errorCode)
{
    unsigned char retval;

    try
    {
        retval = this->feature->getBufferingEnable(*this->protocol, *this->bus, 0);
        SET_ERROR_CODE(ERROR_SUCCESS);
        return retval;
    } catch (FeatureException &fe)
    {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }
}

void FastBufferFeatureAdapter::setBufferingEnable(int *errorCode, unsigned char isEnabled)
{
    try
    {
        this->feature->setBufferingEnable(*this->protocol, *this->bus, 0, isEnabled);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe)
    {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

unsigned int FastBufferFeatureAdapter::getConsecutiveSampleCount(int *errorCode)
{
	unsigned int retval;

	try
	{
		retval = this->feature->getConsecutiveSampleCount(*this->protocol, *this->bus, 0);
		SET_ERROR_CODE(ERROR_SUCCESS);
		return retval;
	}
	catch (FeatureException &fe)
	{
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		return 0;
	}
}

void FastBufferFeatureAdapter::setConsecutiveSampleCount(int *errorCode, unsigned int consecutiveSampleCount)
{
	try
	{
		this->feature->setConsecutiveSampleCount(*this->protocol, *this->bus, 0, consecutiveSampleCount);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe)
	{
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
}
