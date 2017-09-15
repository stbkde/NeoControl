#ifndef SRC_NEOBUS_H
#define SRC_NEOBUS_H

#include "NeoPixelBrightnessBus.h"


typedef NeoPixelBrightnessBus<WS281X_FEATURE, WS281X_METHOD> NeoBusType;


struct StateType 
{
    bool PowerState;
    bool IsPowerSaving = false;
    
    uint16_t FirstPixel = 0;
    uint16_t PixelCount = 0;
    
    RgbColor LastColor;
    RgbColor CurrentColor;
    
    uint8_t LastBrightness = 150;
    uint8_t CurrentBrightness = 150;
};


#endif  // SRC_NEOBUS_H