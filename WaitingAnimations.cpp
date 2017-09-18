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

#include <QList.h>

#include "WaitingAnimations.h"
#include "TWaitAnimation.h"
#include "animations/wait/waitPulseColor.h"

extern TNeoBus * NeoStrip;
extern TState * State;
extern TSettings * Settings;

NeoPixelAnimator * WaitingAnimator = nullptr;


uint16_t _effectState_old = 0; 

struct _animationStateType_old
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};

_animationStateType_old _animationState_old[1];


void CWaitingAnimator::_init_animator()
{
    if (WaitingAnimator) {
        delete WaitingAnimator;
        WaitingAnimator = nullptr;
    }
    
    if (!WaitingAnimator) {
        WaitingAnimator = new NeoPixelAnimator(NeoStrip->PixelCount());
    }
    
    //this->_waitingAnimations = new waitPulseColor;
}

void CWaitingAnimator::StartAnimation()
{
    if (Settings->WaitingAnimation == PULSE_COLOR)
    {
        // ein globales objekt welches mit CWaitingAnimator gesteuert werden kann.
        //waitPulseColor 
    }
}

void CWaitingAnimator::StartAnimation_old()
{
    _effectState_old = 0;
    RgbColor pcolor = RgbColor(5,107,12);
    
    Serial.println("StartAnimation\n");
    
    AnimUpdateCallback _blendAnimUpdate = [=](const AnimationParam& param)
    {
        RgbColor updatedColor = RgbColor::LinearBlend(
            _animationState_old[param.index].StartingColor,
            _animationState_old[param.index].EndingColor,
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
            if (_effectState_old == 0)
            {
                _animationState_old[0].StartingColor = NeoStrip->GetPixelColor(0);
                
                WaitingAnimator->StartAnimation(0, 1000, _blendAnimUpdate);
                _animationState_old[0].EndingColor = pcolor;
            }
            else if (_effectState_old == 1)
            {
                _animationState_old[0].StartingColor = NeoStrip->GetPixelColor(0);
                _animationState_old[0].EndingColor = RgbColor(State->CurrentColor);
                
                WaitingAnimator->StartAnimation(0, 1000, _blendAnimUpdate);
            }
            
            _effectState_old = (_effectState_old + 1) % 2;
            WaitingAnimator->RestartAnimation(param.index);
        }   
    };
    
    WaitingAnimator->StartAnimation(1, 1000, _pulseAnimUpdate);    // time = 1000
}

void CWaitingAnimator::StopAnimation()
{
    WaitingAnimator->StopAll();
}

bool CWaitingAnimator::IsAnimating()
{
    return WaitingAnimator->IsAnimating();
}


void CWaitingAnimator::loop()
{
    if (WaitingAnimator) {
        if (WaitingAnimator->IsAnimating()) 
        {
            WaitingAnimator->UpdateAnimations();
            NeoStrip->Show();
        }
    }
}
