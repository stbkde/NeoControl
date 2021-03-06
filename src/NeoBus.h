/*-------------------------------------------------------------------------
NeoControl

Written by Stephan Beier

-------------------------------------------------------------------------
NeoControl is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

NeoControl is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with NeoPixel.  If not, see
<http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/

#ifndef SRC_NEOBUS_H
#define SRC_NEOBUS_H

#include "NeoPixelBus.h"


#define ON  true
#define OFF false


typedef NeoPixelBus<WS281X_FEATURE, WS281X_METHOD> TNeoBus;


// Waiting animations 1***
#define PULSE_COLOR 1001
#define PULSE_BRIGHTNESS 1002

// Other animations 2***


struct TState 
{
    bool PowerState = OFF;
    
    HslColor LastColor;
    HslColor CurrentColor;
};

// one per strip-part
struct TSettings
{    
    bool PowerSaving = false;
    
    uint16_t FirstPixel = 0;
    uint16_t PixelCount = 0;
    
    uint8_t MinBrightness = 20;
    uint8_t MaxBrightness = 80;
    
    uint16_t WaitingAnimation = PULSE_COLOR; // default
    
    uint16_t FadingTime = 1600;
    String FadeEffect = "smooth";   // smooth, rainbow (, darken)
};


#endif  // SRC_NEOBUS_H
