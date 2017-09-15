#pragma once

#include "config.h"
#include "NeoBus.h"

#include <NeoPixelBrightnessBus.h>
#include <NeoPixelAnimator.h>



class CWaitingAnimator
{
public:
    CWaitingAnimator()
    {
    }
    
    void _init_animator();
    
    RgbColor StripCurrentColor;
    
    void StartAnimation();
    
    void StopAnimation();
    
    bool IsAnimating();
    
    //void loop();
};