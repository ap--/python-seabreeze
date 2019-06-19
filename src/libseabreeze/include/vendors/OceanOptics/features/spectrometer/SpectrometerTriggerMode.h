/***************************************************//**
 * @file    SpectrometerTriggerMode.h
 * @date    August 2009
 * @author  Ocean Optics, Inc.
 *
 * This file describes the different kinds of trigger modes
 * that spectrometers may support.
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

#ifndef SPECTROMETER_TRIGGER_MODE_H
#define SPECTROMETER_TRIGGER_MODE_H

#define SPECTROMETER_TRIGGER_MODE_NORMAL               0x00
#define SPECTROMETER_TRIGGER_MODE_SOFTWARE             0x01
#define SPECTROMETER_TRIGGER_MODE_LEVEL                0x01
#define SPECTROMETER_TRIGGER_MODE_SYNCHRONIZATION      0x02
#define SPECTROMETER_TRIGGER_MODE_HARDWARE             0x03
#define SPECTROMETER_TRIGGER_MODE_EDGE                 0x03
#define SPECTROMETER_TRIGGER_MODE_SINGLE_SHOT          0x04
#define SPECTROMETER_TRIGGER_MODE_SELF_NORMAL          0x80
#define SPECTROMETER_TRIGGER_MODE_SELF_SOFTWARE        0x81
#define SPECTROMETER_TRIGGER_MODE_SELF_SYNCHRONIZATION 0x82
#define SPECTROMETER_TRIGGER_MODE_SELF_HARDWARE        0x83
#define SPECTROMETER_TRIGGER_MODE_DISABLED			   0xFF

#define SPECTROMETER_TRIGGER_MODE_OBP_NORMAL           0x00
#define SPECTROMETER_TRIGGER_MODE_OBP_EXTERNAL         0x01
#define SPECTROMETER_TRIGGER_MODE_OBP_INTERNAL         0x02

namespace seabreeze {

    class SpectrometerTriggerMode {
    public:
        SpectrometerTriggerMode(int mode);
        virtual ~SpectrometerTriggerMode();

        int getTriggerMode();

        /* Overriding equality operator so that modes can be compared. */
        bool operator==(const SpectrometerTriggerMode &that);

    private:
        int triggerMode;
    };

}

#endif /* SPECTROMETER_TRIGGER_MODE_H */
