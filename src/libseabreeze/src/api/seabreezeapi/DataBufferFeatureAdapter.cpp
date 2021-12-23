/***************************************************//**
 * @file    DataBufferFeatureAdapter.cpp
 * @date    October 2017
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
#include "api/seabreezeapi/DataBufferFeatureAdapter.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"

using namespace seabreeze;
using namespace seabreeze::api;

DataBufferFeatureAdapter::DataBufferFeatureAdapter(
    DataBufferFeatureInterface *intf, const FeatureFamily &f,
        Protocol *p, Bus *b, unsigned short instanceIndex)
    : FeatureAdapterTemplate<DataBufferFeatureInterface>(intf, f, p, b, instanceIndex)
{

}

DataBufferFeatureAdapter::~DataBufferFeatureAdapter()
{

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
void DataBufferFeatureAdapter::clearBuffer(int *errorCode)
{
    try
    {
        this->feature->clearBuffer(*this->protocol, *this->bus, 0);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe)
    {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

void DataBufferFeatureAdapter::removeOldestSpectraFromBuffer(int *errorCode, unsigned int numberOfSpectra)
{
    try
    {
        this->feature->removeOldestSpectraFromBuffer(*this->protocol, *this->bus, 0, numberOfSpectra);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe)
    {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

unsigned long DataBufferFeatureAdapter::getNumberOfElements(int *errorCode)
{
    unsigned long retval;

    try
    {
        retval = this->feature->getNumberOfElements(*this->protocol, *this->bus, 0);
        SET_ERROR_CODE(ERROR_SUCCESS);
        return retval;
    } catch (const FeatureException &fe)
    {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }
}

unsigned long DataBufferFeatureAdapter::getBufferCapacity(int *errorCode)
{
    unsigned long retval;

    try
    {
        retval = this->feature->getBufferCapacity(*this->protocol, *this->bus, 0);
        SET_ERROR_CODE(ERROR_SUCCESS);
        return retval;
    } catch (const FeatureException &fe)
    {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }
}

unsigned long DataBufferFeatureAdapter::getBufferCapacityMaximum(int *errorCode)
{
    unsigned long retval;

    try
    {
        retval = this->feature->getBufferCapacityMaximum(*this->protocol, *this->bus, 0);
        SET_ERROR_CODE(ERROR_SUCCESS);
        return retval;
    } catch (const FeatureException &fe)
    {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }
}

unsigned long DataBufferFeatureAdapter::getBufferCapacityMinimum(int *errorCode)
{
    unsigned long retval;

    try
    {
        retval = this->feature->getBufferCapacityMinimum(*this->protocol, *this->bus, 0);
        SET_ERROR_CODE(ERROR_SUCCESS);
        return retval;
    } catch (const FeatureException &fe)
    {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }
}

void DataBufferFeatureAdapter::setBufferCapacity(int *errorCode, unsigned long capacity)
{
    try
    {
        this->feature->setBufferCapacity(*this->protocol, *this->bus, 0, capacity);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe)
    {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}
