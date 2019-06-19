/***************************************************//**
 * @file    FeatureAdapterTemplate.h
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This is a templated wrapper around SeaBreeze Feature
 * instances.  This should make it easier to obtain a
 * particular Feature to call methods against.
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

#ifndef FEATUREADAPTERTEMPLATE_H
#define FEATUREADAPTERTEMPLATE_H

#include "api/seabreezeapi/FeatureAdapterInterface.h"
#include "common/buses/Bus.h"
#include "common/exceptions/IllegalArgumentException.h"
#include "common/features/FeatureFamily.h"
#include "common/protocols/Protocol.h"
#include <string>

namespace seabreeze {
    namespace api {

        template <class T> class FeatureAdapterTemplate
                    : public FeatureAdapterInterface {
        public:
            FeatureAdapterTemplate(T *featureInterface, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex) {
                this->feature = featureInterface;
                this->family = f;
                this->protocol = p;
                this->bus = b;
                this->index = instanceIndex;

                /* Create a unique ID based on the feature type and index.  This
                 * might be expanded in the future to use one of the bytes for
                 * the feature type or index as a module number.
                 */
                this->ID = (family.getType() << 16) | (instanceIndex & 0x00FFFF);

                if(0 == this->feature || 0 == this->protocol || 0 == this->bus) {
                    std::string error("Null feature interface, protocol, or bus is not allowed.");
                    throw IllegalArgumentException(error);
                }
            }
            virtual ~FeatureAdapterTemplate() { /* Do nothing -- others delete feature */ }
            T *getFeature() { return this->feature; }

            virtual FeatureFamily &getFeatureFamily() { return this->family; }

            virtual long getID() { return this->ID; }

        protected:
            T *feature;
            FeatureFamily family;
            Protocol *protocol;
            Bus *bus;
            unsigned short index;
            unsigned long ID;
        };
    }
}

#endif
