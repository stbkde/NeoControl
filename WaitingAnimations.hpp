#pragma once

#include "config.h"

#include <NeoPixelBrightnessBus.h>
#include <NeoPixelAnimator.h>


class PulseColor_Animation
{
public:
    RgbColor StripCurrentColor;
    
    void StartAnimation(NeoPixelBrightnessBus<WS281X_FEATURE, WS281X_METHOD> *_strip);
    
private:
    uint16_t _countPixels;
    uint16_t _effectState = 0; 
    
    struct _animationStateType
    {
        RgbColor StartingColor;
        RgbColor EndingColor;
    };

    _animationStateType _animationState[1];
    
    void _blendAnimUpdate(const AnimationParam& param);
    void _pulseAnimUpdate(const AnimationParam& param);
    
    NeoPixelAnimator *_animator;
    NeoPixelBrightnessBus<WS281X_FEATURE, WS281X_METHOD> *_strip;
};