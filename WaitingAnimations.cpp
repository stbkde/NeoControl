#include "WaitingAnimations.h"

/*
void FadeToRgbColor(uint16_t time, RgbColor targetColor, NeoPixelBrightnessBus<WS281X_FEATURE, WS281X_METHOD> *bus)
{
    Serial.println("Fading to R:"+String(targetColor.R)+", G:"+String(targetColor.G)+", B:"+String(targetColor.B));
    
    AnimEaseFunction easing = NeoEase::Linear;
    
    for (uint16_t pixel = 0; pixel < WS281X_STRIP_COUNT; pixel++)
    {
        RgbColor originalColor = bus->GetPixelColor(pixel); 
        
        AnimUpdateCallback colorAnimUpdate = [=](const AnimationParam& param)
        {
            float progress = easing(param.progress);
            
            RgbColor updatedColor = RgbColor::LinearBlend(originalColor, targetColor, progress);
            bus->SetPixelColor(pixel, updatedColor);
        };
        
        // animations.StartAnimation(pixel, time, colorAnimUpdate);
        WaitingAnimator.StartAnimation(pixel, time/2 +(pixel*time)/pixel/2, colorAnimUpdate, bus); // Do not update all pixels at once but the leftmost twice as fast
    }
};
*/