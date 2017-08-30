/*-------------------------------------------------------------------------
NeoControl

NeoPixelBus library wrapper template class that provides overall control

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

#pragma once

#define ON  true
#define OFF false

#include "NeoPixelBrightnessBus.h"
#include "NeoPixelAnimator.h"

#include "WaitingAnimations.h"



template<typename T_COLOR_FEATURE, typename T_METHOD> class NeoControl : 
    public NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>
{
public:
    NeoControl(uint16_t countPixels, uint8_t pin) :
        NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>(countPixels, pin)
    {
        _brightFadingAnimator = NeoPixelAnimator(countPixels);
        _colorFadingAnimator = NeoPixelAnimator(countPixels);
        _waitingAnimator = NeoPixelAnimator(countPixels);
    }

    NeoControl(uint16_t countPixels, uint8_t pinClock, uint8_t pinData) :
        NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>(countPixels, pinClock, pinData)
    {
        _brightFadingAnimator = NeoPixelAnimator(countPixels);
        _colorFadingAnimator = NeoPixelAnimator(countPixels);
        _waitingAnimator = NeoPixelAnimator(countPixels);
    }

    NeoControl(uint16_t countPixels) :
        NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>(countPixels)
    {
        _brightFadingAnimator = NeoPixelAnimator(countPixels);
        _colorFadingAnimator = NeoPixelAnimator(countPixels);
        _waitingAnimator = NeoPixelAnimator(countPixels);
    }
    
    void setup()
    {
        NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>::Begin();
        
        for (uint16_t pixel=0; pixel<=_countPixels; pixel++)
        {
            NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>::SetPixelColor(pixel, RgbColor(0));
        }
        
        NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>::Show();
        SetRandomSeed();
    }
    
    void loop()
    {
        if (_waitingAnimator.IsAnimating()) 
        {
            _waitingAnimator.UpdateAnimations();
            NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>::Show();
        }
    }
    
    void SetStripColor(RgbColor color)
    {
        if (_waitingAnimator.IsAnimating())
        {
            Serial.println("NEW COLOR and waiting animator IS animating");
            _colorFadingAnimator.StopAll();

            if (_powerState == ON) 
            {
                _currColor = color;
                _nextColor = color;
            }
            else 
            {
                // set black
                _lastColor = color;     // save as last color, wich will be restored at start
                _currColor = RgbColor(0);
                FadeToRgbColor(_colorFadingTime, RgbColor(0));
            }
        }
        else {
            Serial.println("NEW COLOR and waiting animator is NOT animating");
            _colorFadingAnimator.StopAll();
            
            if (_powerState == ON) 
            {
                FadeToRgbColor(_colorFadingTime, color);
            }
            else 
            {
                // set black
                _lastColor = color;    // save as last color, wich will be restored at start
                _currColor = RgbColor(0);
                FadeToRgbColor(_colorFadingTime, RgbColor(0));
            }
        }
    }
    
    void SetStripBrightness(uint8_t targetBrightness)
    {
        if ((targetBrightness >= _minBrightness) && (targetBrightness <= _maxBrightness))
        {
            if (_waitingAnimator.IsAnimating())
            {
                Serial.println("WaitingAnimationRunning == true");
                _brightFadingAnimator.StopAll();

                if (_powerState == ON) 
                {
                    _lastBrightness     = _currentBrightness;
                    _currentBrightness  = targetBrightness;
                    _nextBrightness     = targetBrightness;

                    FadeToBrightness(_brightnessFadingTime, targetBrightness);
                }
                else 
                {
                    _lastBrightness     = targetBrightness;
                }
            }
            else 
            {
                Serial.println("WaitingAnimationRunning == false");
                _brightFadingAnimator.StopAll();

                if (_powerState == ON) 
                {
                    _lastBrightness     = _currentBrightness;
                    _currentBrightness  = targetBrightness;
                    _nextBrightness     = targetBrightness;

                    FadeToBrightness(_brightnessFadingTime, targetBrightness);
                }
                else 
                {
                    _lastBrightness     = targetBrightness;
                }
            }
        }
    }
    
    typename T_COLOR_FEATURE::ColorObject GetLastColor()
    {
        return _lastColor;
    }
    
    void SetLastColor(typename T_COLOR_FEATURE::ColorObject color)
    {
        _lastColor = color;
    }
    
    typename T_COLOR_FEATURE::ColorObject GetCurrentColor()
    {
        return _lastColor;
    }
    
    
    void SetNextColor(typename T_COLOR_FEATURE::ColorObject color)
    {
        _lastColor = color;
    }
    
    
    void SetCurrentColor(typename T_COLOR_FEATURE::ColorObject color)
    {
        _lastColor = color;
    }
    
    typename T_COLOR_FEATURE::ColorObject GetNextColor()
    {
        return _lastColor;
    }
    
    void StartWaitingAnimation()
    {
        //_waitingAnimator.StartAnimation(1, 1000, PulseColorAnimUpdate);
    }
    
    void StopWaitingAnimation()
    {
        _waitingAnimator.StopAll();
        
        if (_powerState == ON)
        {
            //SetStripBrightness(_currentBrightness);
            SetStripColor(_currColor);
        }
        else
        {
            SetStripColor(RgbColor(0));
        }
    }
    
    bool WaitingAnimationRunning() const
    {
        return _waitingAnimator.IsAnimating();
    }
    
    bool PowerState()
    {
        return _powerState;
    }
    
    void PowerOn()
    {
        _powerState = ON;

        if (_lastColor == RgbColor(0)) // after hardreset
        { 
            Serial.println("LastRgbColor ist black. Setting to red");
            _lastColor = RgbColor(255,0,0);
        }

        Serial.println("Received switch on signal...");

        if (_waitingAnimator.IsAnimating()) 
        {
            Serial.println("WaitingAnimationRunning == true");

            _nextBrightness = _lastBrightness;
            _nextColor = _lastColor; // initial color
            _currColor = _lastColor;
        }
        else 
        {
            Serial.println("WaitingAnimationRunning == false");

            SetStripBrightness(_lastBrightness);
            SetStripColor(_lastColor);
            
            _currentBrightness = _lastBrightness;
            _currColor = _lastColor;
        }
    }
    
    void PowerOff()
    {
        _powerState = OFF;

        _lastBrightness = _currentBrightness;
        _lastColor = _currColor;
        Serial.println("Switching OFF and setting LastRgbColor = CurrentRgbColor");

        if (_waitingAnimator.IsAnimating()) 
        {
          _nextColor = RgbColor(0); // setting after animation finished
          _currColor = RgbColor(0);
          _currentBrightness = NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>::GetBrightness();
        }
        else 
        {
          _waitingAnimator.StopAll();
          SetStripColor(RgbColor(0));
          _nextColor = RgbColor(0);
          _currColor = RgbColor(0);
          _currentBrightness = NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>::GetBrightness();
        }
    }

private:
    bool _powerState = OFF;
    
    typename T_COLOR_FEATURE::ColorObject _lastColor = RgbColor(0);
    typename T_COLOR_FEATURE::ColorObject _currColor = RgbColor(0);
    typename T_COLOR_FEATURE::ColorObject _nextColor = RgbColor(0);
    
    uint8_t _lastBrightness     = 150;
    uint8_t _currentBrightness  = 150;
    uint8_t _nextBrightness     = 150;
    
    uint8_t _minBrightness = 20;
    uint8_t _maxBrightness = 255;
    
    NeoPixelAnimator _brightFadingAnimator  = NeoPixelAnimator(1);
    NeoPixelAnimator _colorFadingAnimator   = NeoPixelAnimator(1);
    NeoPixelAnimator _waitingAnimator       = NeoPixelAnimator(1);  
    
    uint16_t _colorFadingTime       = 1000;
    uint16_t _brightnessFadingTime  = 2000;
    
    uint16_t _effectState = 0;
    MyAnimationState_pulse animationState_pulse[1];
    
    unsigned long timerCurrentMillis = 0;
    unsigned long timerLastMillis    = 0;
    
    
    void FadeToBrightness(uint16_t time, int targetBrightness) //since values are centiseconds, 1000 = 10 seconds
    {
        if (targetBrightness < _minBrightness) {
            targetBrightness = _minBrightness;
        }
        
        uint16_t originalBrightness = NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>::GetBrightness();

        AnimEaseFunction easing = NeoEase::Linear;
    
        AnimUpdateCallback animUpdate = [=](const AnimationParam& param)
        {
            int updatedBrightness;
            float progress = easing(param.progress); 
            
            if (progress == 0.00) {
                progress = 0.01;    // correct progress to prevent calculation errors
            }
            
            if (originalBrightness < targetBrightness) 
            {  
                // fading up
                updatedBrightness = (progress * (originalBrightness - targetBrightness));
                updatedBrightness = ~updatedBrightness + 1; // +/-
                updatedBrightness += originalBrightness;
            }
            else if (originalBrightness > targetBrightness) 
            {  
                // fading down
                updatedBrightness = (progress * (targetBrightness - originalBrightness));
                updatedBrightness = ~updatedBrightness + 1; // +/-
                updatedBrightness -= originalBrightness;
                updatedBrightness = ~updatedBrightness + 1;
            }
            
            if (NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>::GetBrightness() != updatedBrightness) 
            {
                NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>::SetBrightness(updatedBrightness);
            }
        };
        
        _brightFadingAnimator.StartAnimation(1, time, animUpdate);
        //animations_PixelCount.StartAnimation(pixel, time/2 +(pixel*time)/PixelCount/2, animUpdate); // Do not update all pixels at once but the leftmost twice as fast

    }
    
/*    
    void PulseColorAnimUpdate(const AnimationParam& param)
    {
        if (param.state == AnimationState_Completed)
        {
            if (_effectState == 0)
            {
                // Fade to effect color
                uint16_t time = 1000;
                animationState_pulse[0].StartingColor = NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>::GetPixelColor(0);
                animationState_pulse[0].EndingColor = RgbColor(20,30,5); // green
        
                _waitingAnimator.StartAnimation(0, time, BlendAnimUpdate2);
            }
            else if (_effectState == 1)
            {
                // Fade to black / current color
                uint16_t time = 1000;
                animationState_pulse[0].StartingColor = NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>::GetPixelColor(0);
                animationState_pulse[0].EndingColor = _currColor;
        
                _waitingAnimator.StartAnimation(0, time, BlendAnimUpdate2);
            }
            _effectState = (_effectState + 1) % 2;
            _waitingAnimator.RestartAnimation(param.index);
        }
    }
*/
    
    /*
    * Needed for some animations for real random
    */
    void SetRandomSeed()
    {
        uint32_t seed;

        // random works best with a seed that can use 31 bits
        // analogRead on a unconnected pin tends toward less than four bits
        seed = analogRead(0);
        delay(1);

        for (int shifts = 3; shifts < 31; shifts += 3)
        {
            seed ^= analogRead(0) << shifts;
            delay(1);
        }
        randomSeed(seed);
    }

protected:
    using NeoPixelBrightnessBus<T_COLOR_FEATURE, T_METHOD>::_countPixels;
    
};

 
