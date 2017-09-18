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

#include <functional>

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
    
    void SetStripColor(const HslColor& color);
    
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
    
    HslColor GetCurrentColor() // const
    {
        return State->CurrentColor;
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
    
    CWaitingAnimator * _waitAnimations;
    NeoPixelAnimator * _colorFadingAnimator;
    
    unsigned long timerCurrentMillis = 0;
    unsigned long timerLastMillis    = 0;
    
    void _init_leds();
    
    void _setStripColor(const HslColor& color);
    
    void _fadeToColor(const HslColor& targetColor);
    
    void _fadeHsl(uint16_t time, const HslColor& targetColor);
    void _fadeRgb(uint16_t time, const HslColor& targetColor);
    
protected:
    uint16_t _pin = 0;
    uint16_t _countPixels = 0;
    
};
