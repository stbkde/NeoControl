#pragma once

/*-------------------------------------------------------------------------
NeoControl

Written by Stephan Beier

-------------------------------------------------------------------------
NeoControl is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

NeoPixelBus is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with NeoPixel.  If not, see
<http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/

#define ON  true
#define OFF false

// More information on https://github.com/Makuna/NeoPixelBus/wiki/NeoPixelBus-object
#define WS281X_FEATURE NeoGrbFeature
#define WS281X_METHOD Neo800KbpsMethod
#define WS281X_STRIP_COUNT 30
#define WS281X_STRIP_PIN 3

#include <NeoPixelBrightnessBus.h>
//#include "NeoAnimator.h"
#include "NeoPixelAnimator.h"

//#include "WaitingAnimations.h"  // is using WaitingAnimator


class NeoControl
{
public:
    NeoControl();
    ~NeoControl();
    
    void Draw()
    {
        this->_strip->Show();
    }
    
    void Show()
    {
        this->_strip->Show();
    }
    
    void setup();    
    void loop();
    
    void SetStripColor(RgbColor color);
    void SetStripBrightness(uint8_t targetBrightness);
    
    // should not be used outside of this class
    // only for testing
    void SetPixelColor(uint16_t pixel, RgbColor color)
    {
        this->_strip->SetPixelColor(pixel, color);
        this->_strip->Show();
    }
    
    void PowerOn();
    void PowerOff();
    
    bool PowerState()
    {
        return _powerState;
    }
    
    uint8_t GetStripPin()
    {
        return _pin;
    }
    
    RgbColor GetLastColor()
    {
        return _lastColor;
    }
    
    //?
    void SetLastColor(RgbColor color)
    {
        _lastColor = color;
    }
    
    RgbColor GetCurrentColor()
    {
        return _lastColor;
    }
    
    //?
    void SetNextColor(RgbColor color)
    {
        _lastColor = color;
    }
    
    //?
    void SetCurrentColor(RgbColor color)
    {
        _lastColor = color;
    }
    
    RgbColor GetNextColor()
    {
        return _lastColor;
    }
    
    void StartWaitingAnimation()
    {
    }
    
    void StopWaitingAnimation()
    {
    }
    

private:
    NeoPixelBrightnessBus<WS281X_FEATURE, WS281X_METHOD> *_strip;
    
    bool _powerState = OFF;
    
    RgbColor _lastColor = RgbColor(0);
    RgbColor _currColor = RgbColor(0);
    RgbColor _nextColor = RgbColor(0);
    
    uint8_t _lastBrightness     = 150;
    uint8_t _currentBrightness  = 150;
    uint8_t _nextBrightness     = 150;
    
    uint8_t _minBrightness = 20;
    uint8_t _maxBrightness = 255;
    
    NeoPixelAnimator *_brightFadingAnimator;
    NeoPixelAnimator *_colorFadingAnimator;
    NeoPixelAnimator *_waitingAnimator;
    
    uint16_t _colorFadingTime       = 1000;
    uint16_t _brightnessFadingTime  = 2000;
    
    unsigned long timerCurrentMillis = 0;
    unsigned long timerLastMillis    = 0;
    
    void _setStripColor(RgbColor color);
    
    void FadeToRgbColor(uint16_t time, RgbColor targetColor)
    {
        AnimEaseFunction easing = NeoEase::Linear;
        
        for (uint16_t pixel = 1; pixel < WS281X_STRIP_COUNT; pixel++)
        {
            RgbColor originalColor = _strip->GetPixelColor(pixel); 
            
            AnimUpdateCallback colorAnimUpdate = [=](const AnimationParam& param)
            {
                float progress = easing(param.progress);
                RgbColor updatedColor = RgbColor::LinearBlend(originalColor, targetColor, progress);
                _strip->SetPixelColor(pixel, updatedColor);
            };
            
            // animations.StartAnimation(pixel, time, colorAnimUpdate);     // starting all at once
            _waitingAnimator->StartAnimation(pixel, time/2 +(pixel*time)/pixel/2, colorAnimUpdate); // Do not update all pixels at once but the leftmost twice as fast
        }
    };
    
    void FadeToBrightness(uint16_t time, uint8_t targetBrightness)
    {
        Serial.println("Fading to brightness " + String(targetBrightness));
        if (targetBrightness < _minBrightness) {
            targetBrightness = _minBrightness;
        }
        uint16_t originalBrightness = this->_strip->GetBrightness();
        
        AnimEaseFunction easing = NeoEase::Linear;
        
        AnimUpdateCallback brightAnimUpdate = [=](const AnimationParam& param)
        {
            int updatedBrightness;
            float progress = easing(param.progress); 
            if (progress == 0.00) {
                progress = 0.01;
            }
            
            if (originalBrightness < targetBrightness) {  // fading up
                updatedBrightness = (progress * (originalBrightness - targetBrightness));
                updatedBrightness = ~updatedBrightness + 1; // +/-
                updatedBrightness += originalBrightness;
            }
            else if (originalBrightness > targetBrightness) {  // fading down
                updatedBrightness = (progress * (targetBrightness - originalBrightness));
                updatedBrightness = ~updatedBrightness + 1; // +/-
                
                updatedBrightness -= originalBrightness;
                updatedBrightness = ~updatedBrightness + 1;
            }

            if(this->_strip->GetBrightness() != updatedBrightness) 
            {
                Serial.println("Setting updated brightness = " + String(updatedBrightness));
                this->_strip->SetBrightness(updatedBrightness);
            }
        };
        
        this->_brightFadingAnimator->StartAnimation(1, time, brightAnimUpdate);
        
    }
    
protected:
    uint16_t _pin = 0;
    uint16_t _countPixels = 0;
    
    
    
};
