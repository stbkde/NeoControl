/*-------------------------------------------------------------------------

Waiting Animation: "Pulse Color"

written by Stephan Beier (stbkde@gmail.com)

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

#include "../../waitPulseColor.h"


extern TNeoBus * NeoStrip;
extern TState * State;
extern TSettings * Settings;

extern NeoPixelAnimator * WaitingAnimator;


uint16_t _effectState = 0; 

struct _animationStateType
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};

_animationStateType _animationState[1];


void waitPulseColor::StartAnimation()
{
    _effectState = 0;
    RgbColor pcolor = RgbColor(20,30,5);
    
    Serial.println("StartAnimation\n");
    
    AnimUpdateCallback _blendAnimUpdate = [=](const AnimationParam& param)
    {
        RgbColor updatedColor = RgbColor::LinearBlend(
            _animationState[param.index].StartingColor,
            _animationState[param.index].EndingColor,
            param.progress);
        
        for (uint16_t pixel = 0; pixel < NeoStrip->PixelCount(); pixel++)
        {
            NeoStrip->SetPixelColor(pixel, updatedColor);
        }
    };
    
    AnimUpdateCallback _pulseAnimUpdate = [=](const AnimationParam& param)
    {
        if (param.state == AnimationState_Completed)
        {
            if (_effectState == 0)
            {
                _animationState[0].StartingColor = NeoStrip->GetPixelColor(0);
                
                WaitingAnimator->StartAnimation(0, 1000, _blendAnimUpdate);
                _animationState[0].EndingColor = pcolor;
            }
            else if (_effectState == 1)
            {
                _animationState[0].StartingColor = NeoStrip->GetPixelColor(0);
                _animationState[0].EndingColor = State->CurrentColor;
                
                WaitingAnimator->StartAnimation(0, 1000, _blendAnimUpdate);
            }
            
            _effectState = (_effectState + 1) % 2;
            WaitingAnimator->RestartAnimation(param.index);
        }   
    };
    
    WaitingAnimator->StartAnimation(1, 1000, _pulseAnimUpdate);    // time = 1000
}