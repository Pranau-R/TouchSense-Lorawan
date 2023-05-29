/*

Module: Catena4610_cMeasurementLoop_fillBuffer.cpp

Function:
        Class for transmitting accumulated measurements.

Copyright:
        See accompanying LICENSE file for copyright and license information.

Author:
        Pranau R, MCCI Corporation   May 2023

*/

#include <Catena_TxBuffer.h>
#include <MCCI_Catena_Iqs620a.h>

#include "Catena4610_cMeasurementLoop.h"

#include <arduino_lmic.h>

using namespace McciCatena;
using namespace McciCatena4610;
using namespace McciCatenaIqs620a;

/*

Name:   McciCatena4610::cMeasurementLoop::fillTxBuffer()

Function:
        Prepare a messages in a TxBuffer with data from current measurements.

Definition:
        void McciCatena4610::cMeasurementLoop::fillTxBuffer(
                cMeasurementLoop::TxBuffer_t& b
                );

Description:
        A format 0x30 message is prepared from the data in the cMeasurementLoop
        object.

*/

void
cMeasurementLoop::fillTxBuffer(
    cMeasurementLoop::TxBuffer_t& b, Measurement const &mData
    )
    {
    gLed.Set(McciCatena::LedPattern::Measuring);

    // initialize the message buffer to an empty state
    b.begin();

    // insert format byte
    b.put(kMessageFormat);

    // the flags in Measurement correspond to the over-the-air flags.
    b.put(std::uint8_t(this->m_data.flags));

    // send Vbat
    if ((this->m_data.flags & Flags::Vbat) !=  Flags(0))
        {
        float Vbat = mData.Vbat;
        gCatena.SafePrintf("Vbat:    %d mV\n", (int) (Vbat * 1000.0f));
        b.putV(Vbat);
        }

    // send Vdd if we can measure it.

    // Vbus is sent as 5000 * v
    if ((this->m_data.flags & Flags::Vcc) !=  Flags(0))
        {
        float Vbus = mData.Vbus;
        gCatena.SafePrintf("Vbus:    %d mV\n", (int) (Vbus * 1000.0f));
        b.putV(Vbus);
        }

    // send boot count
    if ((this->m_data.flags & Flags::Boot) !=  Flags(0))
        {
        b.putBootCountLsb(mData.BootCount);
        }

    if ((this->m_data.flags & Flags::TouchProx) !=  Flags(0))
        {
        int16_t channel0 = mData.touchData.Ch0Data;
        int16_t channel1 = mData.touchData.Ch1Data;
        int16_t channel2 = mData.touchData.Ch2Data;
        int16_t amplitude = mData.amplitude.Amplitude;
        // // Touch Channel Data and Hall Effect Amplitude
        gCatena.SafePrintf("IQS620A:     Ch0: %d  Ch1: %d  Ch2: %d  Amplitude: %d\n",
                channel0,
                channel1,
                channel2,
                amplitude
                );
        b.put2uf(channel0);
        b.put2uf(channel1);
        b.put2uf(channel2);
        b.put2sf(amplitude);
        }

    if ((this->m_data.flags & Flags::TouchCount) !=  Flags(0))
        {
        int16_t touchCount = mData.touchData.touchCount;
        // // Touch Channel Data
        gCatena.SafePrintf("TOUCH COUNT:  %d\n",
                touchCount
                );
        b.put2uf(touchCount);
        }

    gLed.Set(McciCatena::LedPattern::Off);
    }
