/*-------------------------------------------------------------------------

Waiting Animation: "Pulse Color"

written by Stephan Beier (stbkde@gmail.com)

-------------------------------------------------------------------------*/

#include "WaitingAnimations.h"


extern NeoBusType * NeoStrip;
extern NeoPixelAnimator * WaitingAnimator;     // waiting animator !!

extern StateType * State;

uint16_t _effectState = 0; 

struct _animationStateType
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};

_animationStateType _animationState[1];


void CWaitingAnimator::_init_animator()
{
    if (WaitingAnimator) {
        delete WaitingAnimator;
        WaitingAnimator = nullptr;
    }
    
    if (!WaitingAnimator)
    {
        WaitingAnimator = new NeoPixelAnimator(NeoStrip->PixelCount());
    }
}

void CWaitingAnimator::StartAnimation()
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

void CWaitingAnimator::StopAnimation()
{
    WaitingAnimator->StopAll();
}

bool CWaitingAnimator::IsAnimating()
{
    return WaitingAnimator->IsAnimating();
}
