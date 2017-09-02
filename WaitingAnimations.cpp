#include "WaitingAnimations.h"


void PulseColor_Animation::StartAnimation(NeoPixelBrightnessBus<WS281X_FEATURE, WS281X_METHOD> *pixelBus)
{
    this->_strip = pixelBus;
    _countPixels = this->_strip->PixelCount();
    
    this->_animator->StartAnimation(1, 1000, static_cast<void(*)()>(_pulseAnimUpdate));
}


void PulseColor_Animation::_blendAnimUpdate(const AnimationParam& param)
{
    //Serial.println("BlendAnimUpdate...");
    RgbColor updatedColor = RgbColor::LinearBlend(
                                        _animationState[param.index].StartingColor,
                                        _animationState[param.index].EndingColor,
                                        param.progress);

    for (uint16_t pixel = 0; pixel < _countPixels; pixel++)
    {
        this->_strip->SetPixelColor(pixel, updatedColor);
    }
}

void PulseColor_Animation::_pulseAnimUpdate(const AnimationParam& param)
{
    if (param.state == AnimationState_Completed)
    {
        if (_effectState == 0)
        {
            //Serial.println("effectState = 0");
            uint16_t time = 1000;
   
            _animationState[0].StartingColor = this->_strip->GetPixelColor(0);
            _animationState[0].EndingColor = RgbColor(20,30,5);
    
            this->_animator->StartAnimation(0, time, _blendAnimUpdate);
        }
        else if (_effectState == 1)
        {
            //Serial.println("effectState = 1");
            uint16_t time = 1000;
 
            _animationState[0].StartingColor = this->_strip->GetPixelColor(0);
            _animationState[0].EndingColor = StripCurrentColor;
    
            this->_animator->StartAnimation(0, time, _blendAnimUpdate);
        }
        
        _effectState = (_effectState + 1) % 2;
        this->_animator->RestartAnimation(param.index);
    }
}
