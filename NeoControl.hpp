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
#pragma once

#include "config.h"
#include "NeoBus.h"

#include <NeoPixelBrightnessBus.h>
#include <NeoPixelAnimator.h>
#include "WaitingAnimations.h"


extern TNeoBus * NeoStrip;
extern TState * State;
extern TSettings * Settings;


class NeoControl
{
public:
    NeoControl(uint16_t pixelcount, uint8_t pin);
    
    void setup();
    
    void loop();
    
    void SetStripColor(RgbColor color);
    
    void SetStripBrightness(uint8_t targetBrightness);
    
    void PowerOn();
    
    void PowerOff();
    
    bool PowerState() // const
    {
        return State->PowerState;
    }
    
    void printInfo();
    
    uint8_t GetStripPin() // const
    {
        return _pin;
    }
    
    RgbColor GetLastColor() // const
    {
        return State->LastColor;
    }
    
    //?
    void SetLastColor(RgbColor color)
    {
        State->LastColor = color;
    }
    
    RgbColor GetCurrentColor() // const
    {
        return State->CurrentColor;
    }
    
    //?
    void SetCurrentColor(RgbColor color)
    {
        State->CurrentColor = color;
    }
    
    void StartWaitingAnimation()
    {
        printInfo();
        _waitAnimations->StartAnimation_old();
    }
    
    void StopWaitingAnimation()
    {
        _waitAnimations->StopAnimation();
        SetStripColor(State->CurrentColor);
    }
    
    void SetPowerSavingMode(bool isEnabled)
    {
        _powerSavingMode = isEnabled;
    }
    
    bool GetPowerSavingMode()
    {
        return _powerSavingMode;
    }
    

private:
    bool _powerSavingMode = false;
    
    uint8_t _minBrightness = 20;
    uint8_t _maxBrightness = 255;
    
    CWaitingAnimator * _waitAnimations;
    
    NeoPixelAnimator * _brightFadingAnimator;
    NeoPixelAnimator * _colorFadingAnimator;
    
    uint16_t _colorFadingTime       = 1000;
    uint16_t _brightnessFadingTime  = 2000;
    
    unsigned long timerCurrentMillis = 0;
    unsigned long timerLastMillis    = 0;
    
    void _init_leds();
    
    void _setStripColor(RgbColor color);
    
    void FadeToRgbColor(uint16_t time, RgbColor targetColor);
    
    void FadeToBrightness(uint16_t time, uint8_t targetBrightness);
    
protected:
    uint16_t _pin = 0;
    uint16_t _countPixels = 0;
    
};

//NeoControl StripControl(30, 3);
 