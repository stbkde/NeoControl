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
    this->_brightFadingAnimator = new NeoPixelAnimator(_countPixels);
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
    
    if (this->_brightFadingAnimator->IsAnimating()) 
    {
        this->_brightFadingAnimator->UpdateAnimations();
        NeoStrip->Show();
    }
    
    _waitAnimations->loop();
}
    
void NeoControl::SetStripColor(const HslColor& color)
{
    RgbColor tcolor(color);
    
    Serial.println("NeoControl::SetStripColor(RgbColor) | Set to color rgb: " + State->LastColor.toString(','));
    
    if (_waitAnimations->IsAnimating())
    {
        if (State->PowerState == ON) 
        {
            // RgbColor
            State->LastColor = State->CurrentColor;
            State->CurrentColor = tcolor;
            
            // HslColor
            State->LastColor_hsl = State->CurrentColor_hsl;
            State->CurrentColor = color;
        }
        else 
        {
            // set black
            
            // RgbColor
            State->LastColor = tcolor;     // save as last color, wich will be restored at start
            State->CurrentColor = RgbColor(0);
            
            // HslColor
            State->LastColor_hsl = color;
            State->CurrentColor = HslColor(0);
            
            //_setStripColor(RgbColor(0));
            FadeToRgbColor(_colorFadingTime, RgbColor(0));
        }
    }
    else
    {
        Serial.println("NeoControl::SetStripColor(RgbColor) | Waiting animator is OFF");
        this->_colorFadingAnimator->StopAll();
        
        if (State->PowerState == ON)
        {
            // RgbColor
            State->LastColor = State->CurrentColor;
            State->CurrentColor = tcolor;
            
            // HslColor
            State->LastColor_hsl = State->CurrentColor_hsl;
            State->CurrentColor_hsl = color;
            
            this->_colorFadingAnimator->StopAll();
            FadeToRgbColor(_colorFadingTime, tcolor);
        }
        else 
        {
            Serial.println("NeoControl::SetStripColor(RgbColor) | Power State is OFF");
            // set black
            
            // RgbColor
            State->LastColor = tcolor;    // save as last color, wich will be restored at start
            State->CurrentColor = RgbColor(0);
            
            // HslColor
            State->LastColor_hsl = color;
            State->CurrentColor_hsl = HslColor(0);
            
            FadeToRgbColor(_colorFadingTime, RgbColor(0));
        }
    }
}

void NeoControl::SetStripColor(const HsbColor& color)
{
     SetStripColor(HslColor(color));
}

void NeoControl::SetStripColor(const RgbColor& color)
{
     SetStripColor(HslColor(color));
}
    
void NeoControl::SetStripBrightness(uint8_t targetBrightness)
{
    if ((targetBrightness >= _minBrightness) && (targetBrightness <= _maxBrightness))
    {
        if (_waitAnimations->IsAnimating())
        {
            //Serial.println("NeoControl::SetStripBrightness | WaitingAnimationRunning == true");
            this->_brightFadingAnimator->StopAll();
            
            if (State->PowerState == ON) 
            {
                State->LastBrightness     = State->CurrentBrightness;
                State->CurrentBrightness  = targetBrightness;
                
                FadeToBrightness(_brightnessFadingTime, targetBrightness);
            }
            else 
            {
                State->LastBrightness     = targetBrightness;
            }
        }
        else 
        {
            //Serial.println("NeoControl::SetStripBrightness | WaitingAnimationRunning == false");
            this->_brightFadingAnimator->StopAll();
            
            if (State->PowerState == ON) 
            {
                State->LastBrightness     = State->CurrentBrightness;
                State->CurrentBrightness  = targetBrightness;
                
                FadeToBrightness(_brightnessFadingTime, targetBrightness);
            }
            else 
            {
                State->LastBrightness     = targetBrightness;
            }
        }
    }
}

void NeoControl::PowerOn()
{   
    if (State->PowerState == OFF) 
    {   
        Serial.println("NeoControl::PowerOn() | Received switch on signal...");
        State->PowerState = ON;
        
        // RgbColor
        if (State->LastColor == RgbColor(0)) { // after hardreset
            State->LastColor = RgbColor(85,255,0);
        }
        
        // HslColor
        if (State->LastColor_hsl == HslColor(0)) { // after hardreset
            State->LastColor_hsl = HslColor(100,100,50);
        }
        
        if (State->LastBrightness < 50) {
            State->LastBrightness += 100;
        }
        
        if (_waitAnimations->IsAnimating()) 
        {
            Serial.println("NeoControl::PowerOn() | WaitingAnimationRunning == true");
            
            State->CurrentBrightness = State->LastBrightness;
            // RgbColor
            State->CurrentColor = State->LastColor;
            // HslColor
            State->CurrentColor_hsl = State->LastColor_hsl;
        }
        else 
        {            
            SetStripBrightness(State->LastBrightness);
            SetStripColor(State->LastColor_hsl);
        }
    }
}

void NeoControl::PowerOff()
{
    if (State->PowerState == ON)
    {
        Serial.println("NeoControl::PowerOff() | Switching OFF and setting LastRgbColor = CurrentRgbColor");
        
        if (_waitAnimations->IsAnimating()) 
        {
            // RgbColor
            State->LastColor = State->CurrentColor;
            State->CurrentColor = RgbColor(0);
            
            // HslColor
            State->LastColor_hsl = State->CurrentColor_hsl;
            State->CurrentColor_hsl = HslColor(0);
            
            State->LastBrightness = State->CurrentBrightness;
            State->CurrentBrightness = NeoStrip->GetBrightness();
        }
        else 
        {
            SetStripColor(HslColor(0));
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
    Serial.println("*********** Brightness ***********************");
    Serial.println("* Last Brightness State: " + String(State->LastBrightness));
    Serial.println("* Current Brightness State: " + String(State->CurrentBrightness));
    Serial.println("*");
    Serial.println("*********** RGB Values ***********************");
    Serial.println("* Last Color: HSL:" + String(State->LastColor_hsl.toString(',')));
    Serial.println("* Current Color: HSL:" + String(State->CurrentColor_hsl.toString(',')));
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
        Serial.println("ERROR CREATING STRIP");
    }
    Serial.println("NeoControl::_init_leds() | Done!");

}

void NeoControl::FadeToHslColor(uint16_t time, const HslColor& targetColor)
{
    Serial.println("NeoControl::FadeToRgbColor(uint16_t time, RgbColor targetColor)");
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
            HslColor updatedColor = HslColor::LinearBlend<NeoHueBlendShortestDistance>(originalColor, targetColor, progress);
            NeoStrip->SetPixelColor(pixel, updatedColor);
        };
        
        // starting all at once
        _colorFadingAnimator->StartAnimation(pixel, time, colorAnimUpdate);
        
        // Do not update all pixels at once but the leftmost twice as fast
        //_colorFadingAnimator->StartAnimation(pixel, time/2 +(pixel*time)/pixel/2, colorAnimUpdate); 
    }
};

void NeoControl::FadeToRgbColor(uint16_t time, const RgbColor& targetColor)
{
    Serial.println("NeoControl::FadeToRgbColor(uint16_t time, RgbColor targetColor)");
    AnimEaseFunction easing = NeoEase::Linear;
    
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
            NeoStrip->SetPixelColor(pixel, updatedColor);
        };
        
        // starting all at once
        _colorFadingAnimator->StartAnimation(pixel, time, colorAnimUpdate);
        
        // Do not update all pixels at once but the leftmost twice as fast
        //_colorFadingAnimator->StartAnimation(pixel, time/2 +(pixel*time)/pixel/2, colorAnimUpdate); 
    }
};

void NeoControl::FadeToBrightness(uint16_t time, uint8_t targetBrightness)
{
    if (targetBrightness < _minBrightness) {
        targetBrightness = _minBrightness;
    }
    uint16_t originalBrightness = NeoStrip->GetBrightness();
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
        
        if (NeoStrip->GetBrightness() != updatedBrightness) 
        {
            NeoStrip->SetBrightness(updatedBrightness);
        }
    };
    
    if (originalBrightness != targetBrightness) 
    {
        Serial.println("Fading from " + String(originalBrightness) + " to brightness " + String(targetBrightness) + " with time ms " + String(time));
        this->_brightFadingAnimator->StartAnimation(1, time, brightAnimUpdate);   
    }
}

void NeoControl::_setStripColor(const HslColor& color)
{
    Serial.println("NeoControl::_setStripColor(RgbColor color)");
    for (uint16_t pixel=0; pixel<=_countPixels; pixel++)
    {
        NeoStrip->SetPixelColor(pixel, RgbColor(color));
    }
    NeoStrip->Show();
    
    printInfo();
}
