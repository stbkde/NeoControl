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

#include "NeoControl.h" 
#include "NeoBus.h"

TNeoBus * NeoStrip = nullptr;
TState * State = nullptr;
TSettings * Settings = nullptr;


NeoControl::NeoControl(uint16_t pixelcount, uint8_t pin) :
    _pin(pin),
    _countPixels(pixelcount)
{
    this->_colorFadingAnimator  = new NeoPixelAnimator(_countPixels);
    this->_waitAnimations = new CWaitingAnimator;
    
    Settings = new TSettings;
    State = new TState;
}

void NeoControl::setup() 
{
    Serial.println("NeoControl::setup() | Strip setup...");
    _init_leds();
    
    _waitAnimations->_init_animator();
}

void NeoControl::loop()
{
    if (this->_colorFadingAnimator->IsAnimating()) 
    {
        this->_colorFadingAnimator->UpdateAnimations();
        NeoStrip->Show();
    }
    
    _waitAnimations->loop();
}
    
void NeoControl::SetStripColor(const HslColor& color)
{
    HslColor tcolor = color;
 
    if (tcolor.L > Settings->MaxBrightness)
        tcolor.L = Settings->MaxBrightness;
    
    if (_waitAnimations->IsAnimating())
    {
        //Serial.println("NeoControl::SetStripColor(HslColor) | Waiting animator is ON");
        
        if (State->PowerState == ON) 
        {
            //Serial.println("NeoControl::SetStripColor(HslColor) | Power State is ON");
        
            State->LastColor = State->CurrentColor;
            State->CurrentColor = tcolor;
        }
        else 
        {
            //Serial.println("NeoControl::SetStripColor(HslColor) | Power State is OFF");
            
            State->LastColor = tcolor;
        }
    }
    else
    {
        //Serial.println("NeoControl::SetStripColor(HslColor) | Waiting animator is OFF");
        
        this->_colorFadingAnimator->StopAll();
        
        if (State->PowerState == ON)
        {
            //Serial.println("NeoControl::SetStripColor(HslColor) | Power State is ON");
            
            State->LastColor = State->CurrentColor;
            State->CurrentColor = tcolor;
            
            this->_colorFadingAnimator->StopAll();
            _fadeToColor(tcolor);
        }
        else 
        {
            //Serial.println("NeoControl::SetStripColor(HslColor) | Power State is OFF");
            
            State->LastColor = tcolor;
        }
    }
    
    printInfo();
}

void NeoControl::PowerOn()
{   
    if (State->PowerState == OFF) 
    {   
        //Serial.println("NeoControl::PowerOn() | Received switch on signal...");
        State->PowerState = ON;
        
        if (State->LastColor == HslColor(0)) { // after hardreset
            State->LastColor = HslColor(100,100,50);  // default light yellow
        }
        
        if (_waitAnimations->IsAnimating()) 
        {
            //Serial.println("NeoControl::PowerOn() | WaitingAnimationRunning == true");
            
            State->CurrentColor = State->LastColor;
        }
        else 
        {   
            State->CurrentColor = State->LastColor;
            _fadeToColor(State->CurrentColor);
        }
    }
}

void NeoControl::PowerOff()
{
    if (State->PowerState == ON)
    {
        //Serial.println("NeoControl::PowerOff() | Switching OFF");
        
        if (_waitAnimations->IsAnimating()) 
        {
            State->LastColor = State->CurrentColor;
            State->CurrentColor = HslColor(0);
        }
        else 
        {
            State->LastColor = State->CurrentColor;
            State->CurrentColor = HslColor(0);
            _fadeToColor(HslColor(0));
        }
        
        State->PowerState = OFF;
    }
}


void NeoControl::printInfo() {
    Serial.println("*********** Strip Info ***********************");
    Serial.println("* LED count: " + String(NeoStrip->PixelCount()));
    Serial.println("*\n");
    
    Serial.println("*********** Color Info ***********************");
    Serial.println("*");
    Serial.println("*********** HSL Color Values ***********************");
    Serial.println("* Last Color: HSL:" + String(State->LastColor.toString(',')));
    Serial.println("* Current Color: HSL:" + String(State->CurrentColor.toString(',')));
    Serial.println("*");
}



/** private **/

void NeoControl::_init_leds()
{
    if (NeoStrip) {
        NeoStrip->ClearTo(0);
        NeoStrip->Show();
        delete NeoStrip;
        NeoStrip = nullptr;
    }

    if (!_countPixels) {
        _countPixels = 1;
    }
    NeoStrip = new TNeoBus(_countPixels);

    if (NeoStrip) {
        NeoStrip->Begin();
        
        Serial.println("NeoControl::_init_leds() | Strip created! Setting color directly to black");
        for (uint16_t pixel=0; pixel<=_countPixels; pixel++)
        {
            NeoStrip->SetPixelColor(pixel, HslColor(0));
        }
        
        Serial.println("NeoControl::_init_leds() | Strip-Show ;)");
        NeoStrip->Show();
    } else {
        Serial.println("Error creating strip !!");
    }
    Serial.println("NeoControl::_init_leds() | Done!");

}

void NeoControl::_fadeToColor(const HslColor& targetColor)
{
    if (Settings->FadeEffect == "smooth") {
        _fadeRgb(Settings->FadingTime, targetColor);
    }
    else if (Settings->FadeEffect == "rainbow") {
        _fadeHsl(Settings->FadingTime, targetColor);
    }
    else if (Settings->FadeEffect == "darken") {
        _fadeHsl(Settings->FadingTime, targetColor);
    }
}

void NeoControl::_fadeHsl(uint16_t time, const HslColor& targetColor)
{
    AnimEaseFunction easing = NeoEase::Linear;
    
    uint16_t stepcount = 1;
    
    if ((State->PowerState == ON) && (_powerSavingMode == ON))
        stepcount = 2;
    
    for (uint16_t pixel = 0; pixel < NeoStrip->PixelCount(); pixel += stepcount)
    {
        HslColor originalColor = NeoStrip->GetPixelColor(pixel); 
        
        AnimUpdateCallback colorAnimUpdate = [=](const AnimationParam& param)
        {
            float progress = easing(param.progress);
            
            if (Settings->FadeEffect == "rainbow") 
            {
                HslColor updatedColor = HslColor::LinearBlend<NeoHueBlendLongestDistance>(originalColor, 
                                                                                       targetColor,
                                                                                       progress);
                NeoStrip->SetPixelColor(pixel, updatedColor);
            }
            else if (Settings->FadeEffect == "darken") 
            {
                HslColor updatedColor = HslColor::LinearBlend<NeoHueBlendShortestDistance>(originalColor, 
                                                                                       targetColor,
                                                                                       progress);
                if (progress > 0.5f) {
                    updatedColor = HslColor(updatedColor.H, updatedColor.S, (1.0f - progress));
                }
                else if (progress > 0.5f) {
                    updatedColor = HslColor(updatedColor.H, updatedColor.S, progress);
                }
                NeoStrip->SetPixelColor(pixel, updatedColor);
            }
            //      NeoHueBlendShortestDistance
            //      NeoHueBlendLongestDistance
            //      NeoHueBlendClockwiseDirection
            //      NeoHueBlendCounterClockwiseDirection
            
            // More informations: https://github.com/Makuna/NeoPixelBus/wiki/NeoHueBlend-objects
        };
        
        // starting all at once
        _colorFadingAnimator->StartAnimation(pixel, time, colorAnimUpdate);
        
        // Do not update all pixels at once but the leftmost twice as fast
        //_colorFadingAnimator->StartAnimation(pixel, time/2 +(pixel*time)/pixel/2, colorAnimUpdate); 
    }
};

void NeoControl::_fadeRgb(uint16_t time, const HslColor& targetColor)
{
    AnimEaseFunction easing = NeoEase::Linear;
    
    RgbColor targetColorRgb(targetColor);
    
    uint16_t stepcount = 1;
    
    if ((State->PowerState == ON) && (_powerSavingMode == ON))
        stepcount = 2;
    
    for (uint16_t pixel = 0; pixel < NeoStrip->PixelCount(); pixel += stepcount)
    {
        RgbColor originalColor = NeoStrip->GetPixelColor(pixel); 
        
        AnimUpdateCallback colorAnimUpdate = [=](const AnimationParam& param)
        {
            float progress = easing(param.progress);
            RgbColor updatedColor = RgbColor::LinearBlend(originalColor, targetColor, progress);
            
            NeoStrip->SetPixelColor(pixel, HslColor(updatedColor));
        };
        
        // starting all at once
        _colorFadingAnimator->StartAnimation(pixel, time, colorAnimUpdate);
        
        // Do not update all pixels at once but the leftmost twice as fast
        //_colorFadingAnimator->StartAnimation(pixel, time/2 +(pixel*time)/pixel/2, colorAnimUpdate); 
    }
};

void NeoControl::_setStripColor(const HslColor& color)
{
    HslColor tcolor = color;
    
    if (tcolor.L > Settings->MaxBrightness)
        tcolor.L = Settings->MaxBrightness;
    
    for (uint16_t pixel=0; pixel<=_countPixels; pixel++)
    {
        NeoStrip->SetPixelColor(pixel, tcolor);
    }
    NeoStrip->Show();
    
    printInfo();
}
