/***************************************************//**
 * @file    FlameXDFastBufferFeature.cpp
 * @date    February 2017
 * @author  Ocean Optics, Inc.
 *
 * This feature provides an interface to the fast buffer functions
 * for the FlameX. It is used in conjunction with the data buffer feature
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
#include "vendors/OceanOptics/features/fast_buffer/FlameXFastBufferFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/FastBufferProtocolInterface.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPFastBufferProtocol.h"
#include "common/exceptions/FeatureControlException.h"
#include "api/seabreezeapi/FeatureFamilies.h"


using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace seabreeze::api;
using namespace std;

FlameXFastBufferFeature::FlameXFastBufferFeature()
{

    this->protocols.push_back(new OBPFastBufferProtocol());
}

FlameXFastBufferFeature::~FlameXFastBufferFeature() {

}
