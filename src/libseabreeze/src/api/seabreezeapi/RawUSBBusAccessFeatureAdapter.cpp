/***************************************************//**
 * @file    RawUSBBusAccessFeatureAdapter.cpp
 * @date    February 2015
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze RawUSBBusAccessFeatureInterface instances.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2015, Ocean Optics Inc
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
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"
#include "api/seabreezeapi/RawUSBBusAccessFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <vector>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

RawUSBBusAccessFeatureAdapter::RawUSBBusAccessFeatureAdapter(
        RawUSBBusAccessFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<RawUSBBusAccessFeatureInterface>(intf, f, p, b, instanceIndex)
{

    /* Nothing else to do here, the initialization list takes care of it */
}

RawUSBBusAccessFeatureAdapter::~RawUSBBusAccessFeatureAdapter()
{
    /* This is just a wrapper around existing instances -- nothing to delete */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
int RawUSBBusAccessFeatureAdapter::readUSB(int *errorCode, unsigned char *buffer, unsigned int bufferLength, unsigned char endpoint)
{
    int charactersCopied = 0;

    std::vector<unsigned char> data(bufferLength);

    try
    {
    	// FIXME: This is probably not any nastier than OOIUSBInterface *__seabreeze_getUSB
    	//        in SeaBreezeWrapper, but still, it takes the first bus and assumes
    	//        it is USB. This should do a search by family on the bus and then cast it.
        data = this->feature->readUSB(dynamic_cast<USBInterface *>(this->bus), endpoint, bufferLength);
        unsigned int characters = (unsigned int) data.size();
        charactersCopied = (characters < bufferLength) ? characters : bufferLength;
        memcpy(buffer, &((data)[0]), characters * sizeof(unsigned char));

        SET_ERROR_CODE(ERROR_SUCCESS);
    }
    catch (const FeatureException &fe)
    {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }
    return charactersCopied;
}

int RawUSBBusAccessFeatureAdapter::writeUSB(int *errorCode, unsigned char *buffer, unsigned int bufferLength, unsigned char endpoint)
{
    int charactersCopied = 0;

    vector<unsigned char> *charVector = new vector<unsigned char>;
    charVector->resize(bufferLength);
    memcpy(&((*charVector)[0]), buffer, bufferLength * sizeof(unsigned char));

    try
    {
        // FIXME: This is probably not any nastier than OOIUSBInterface *__seabreeze_getUSB
    	//        in SeaBreezeWrapper, but still, it takes the first bus and assumes
    	//        it is USB. This should do a search by family on the bus and then cast it.
        charactersCopied = this->feature->writeUSB(dynamic_cast<USBInterface *>(this->bus), endpoint, *charVector);
        delete charVector;
        SET_ERROR_CODE(ERROR_SUCCESS);
    }
    catch (const FeatureException &fe)
    {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        delete charVector;
        return 0;
    }

    return charactersCopied;
}
