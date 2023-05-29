/*

Name:   Catena4610-TouchSense-Lorawan.h

Function:
        Global linkage for Catena4610-TouchSense-Lorawan.ino

Copyright:
        See accompanying LICENSE file for copyright and license information.

Author:
        Pranau R, MCCI Corporation   May 2023

*/

#ifndef _Catena4610-TouchSense-Lorawan_h_
# define _Catena4610-TouchSense-Lorawan_h_

#pragma once

#include <Catena.h>
#include <Catena_Led.h>
#include <Catena_Mx25v8035f.h>
#include <Catena_Timer.h>
#include <MCCI_Catena_Iqs620a.h>
#include <SPI.h>
#include "Catena4610_cMeasurementLoop.h"

using namespace McciCatenaIqs620a;

//  The global clock object

extern  McciCatena::Catena                      gCatena;
extern  McciCatena::cTimer                      ledTimer;
extern  McciCatena::Catena::LoRaWAN             gLoRaWAN;
extern  McciCatena::StatusLed                   gLed;

extern  SPIClass                                gSPI2;
extern  McciCatena4610::cMeasurementLoop        gMeasurementLoop;

// The Touch Sensor
extern cIQS620A                                 gIqs620a;

//  The flash
extern  McciCatena::Catena_Mx25v8035f           gFlash;

#endif // !defined(_Catena4610-TouchSense-Lorawan_h_)
