#include "NeoControl.h" 
    
NeoControl::NeoControl(bool b)
{
    this->_strip = new NeoPixelBrightnessBus<WS281X_FEATURE, WS281X_METHOD>(WS281X_STRIP_COUNT, WS281X_STRIP_PIN);
    
    _pin            = WS281X_STRIP_PIN;
    _countPixels    = WS281X_STRIP_COUNT;
    
    this->_brightFadingAnimator = new NeoPixelAnimator(_countPixels);
    this->_colorFadingAnimator  = new NeoPixelAnimator(_countPixels);
    this->_waitingAnimator      = new NeoPixelAnimator(_countPixels);
}

NeoControl::~NeoControl() {
    delete(this->_strip);
}

void NeoControl::setup() 
{
    this->_strip->Begin();
    
    for (uint16_t pixel=0; pixel<=_countPixels; pixel++)
    {
        this->_strip->SetPixelColor(pixel, RgbColor(0));
    }
    
    this->_strip->Show();
}

void NeoControl::loop()
{
    if (this->_colorFadingAnimator->IsAnimating()) 
    {
        this->_colorFadingAnimator->UpdateAnimations();
        this->_strip->Show();
    }
    
    if (this->_brightFadingAnimator->IsAnimating()) 
    {
        this->_brightFadingAnimator->UpdateAnimations();
        this->_strip->Show();
    }
    
    if (this->_waitingAnimator->IsAnimating()) 
    {
        this->_waitingAnimator->UpdateAnimations();
        this->_strip->Show();
    }
}
    
void NeoControl::SetStripColor(RgbColor color)
{
    Serial.println("Set to color rgb: " + _lastColor.toString(','));
    if (this->_waitingAnimator->IsAnimating())
    {
        if (_powerState == ON) 
        {
            _lastColor = _currColor;
            _currColor = color;
        }
        else 
        {
            // set black
            _lastColor = color;     // save as last color, wich will be restored at start
            _currColor = RgbColor(0);
            
            _setStripColor(RgbColor(0));
            // ?? FadeToRgbColor(_colorFadingTime, RgbColor(0));
        }
    }
    else {
        this->_colorFadingAnimator->StopAll();
        
        if (_powerState == ON)
        {
            _lastColor = _currColor;
            _currColor = color;
            
            this->_colorFadingAnimator->StopAll();
            FadeToRgbColor(_colorFadingTime, color);
        }
        else 
        {
            // set black
            _lastColor = color;    // save as last color, wich will be restored at start
            _currColor = RgbColor(0);
            
            _setStripColor(RgbColor(0));
            //FadeToRgbColor(_colorFadingTime, RgbColor(0), this->_strip);
        }
    }
}

void NeoControl::SetStripBrightness(uint8_t targetBrightness)
{
    if ((targetBrightness >= _minBrightness) && (targetBrightness <= _maxBrightness))
    {
        if (this->_waitingAnimator->IsAnimating())
        {
            Serial.println("WaitingAnimationRunning == true");
            this->_brightFadingAnimator->StopAll();
            
            if (_powerState == ON) 
            {
                _lastBrightness     = _currentBrightness;
                _currentBrightness  = targetBrightness;
                
                //this->_strip->SetBrightness(targetBrightness);
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
            this->_brightFadingAnimator->StopAll();
            
            if (_powerState == ON) 
            {
                _lastBrightness     = _currentBrightness;
                _currentBrightness  = targetBrightness;
                
                //this->_strip->SetBrightness(targetBrightness);
                FadeToBrightness(_brightnessFadingTime, targetBrightness);
            }
            else 
            {
                _lastBrightness     = targetBrightness;
            }
        }
    }
}

void NeoControl::PowerOn()
{   
    if (_powerState == OFF) 
    {   
        Serial.println("Received switch on signal...");
        _powerState = ON;
        
        if (_lastColor == RgbColor(0)) { // after hardreset{ 
            _lastColor = RgbColor(255,0,0);
        }
        if (_lastBrightness < 50) {
            _lastBrightness += 100;
        }
        
        if (this->_waitingAnimator->IsAnimating()) 
        {
            Serial.println("WaitingAnimationRunning == true");
            
            _currentBrightness = _lastBrightness;
            _currColor = _lastColor;
        }
        else 
        {            
            SetStripBrightness(_lastBrightness);
            SetStripColor(_lastColor);
        }
    }
}

void NeoControl::PowerOff()
{
    if (_powerState == ON)
    {
        Serial.println("Switching OFF and setting LastRgbColor = CurrentRgbColor");
        
        if (this->_waitingAnimator->IsAnimating()) 
        {
            _lastColor = _currColor;
            _currColor = RgbColor(0);
            
            _lastBrightness = _currentBrightness;
            _currentBrightness = this->_strip->GetBrightness();
            
        }
        else 
        {
            SetStripColor(RgbColor(0));
        }
        
        _powerState = OFF;
    }
}

void NeoControl::FadeToRgbColor(uint16_t time, RgbColor targetColor)
{
    AnimEaseFunction easing = NeoEase::Linear;
    
    for (uint16_t pixel = 1; pixel < WS281X_STRIP_COUNT; pixel++)
    {
        RgbColor originalColor = _strip->GetPixelColor(pixel); 
        
        AnimUpdateCallback colorAnimUpdate = [=](const AnimationParam& param)
        {
            float progress = easing(param.progress);
            RgbColor updatedColor = RgbColor::LinearBlend(originalColor, targetColor, progress);
            _strip->SetPixelColor(pixel, updatedColor);
        };
        
        // animations.StartAnimation(pixel, time, colorAnimUpdate);     // starting all at once
        _colorFadingAnimator->StartAnimation(pixel, time/2 +(pixel*time)/pixel/2, colorAnimUpdate); // Do not update all pixels at once but the leftmost twice as fast
    }
};

void NeoControl::FadeToBrightness(uint16_t time, uint8_t targetBrightness)
{
    if (targetBrightness < _minBrightness) {
        targetBrightness = _minBrightness;
    }
    uint16_t originalBrightness = this->_strip->GetBrightness();
    AnimEaseFunction easing = NeoEase::Linear;
    
    Serial.println("Fading from " + String(originalBrightness) + " to brightness " + String(targetBrightness) + " with time ms " + String(time));

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
        
        if (this->_strip->GetBrightness() != updatedBrightness) 
        {
            //Serial.println("Setting updated brightness = " + String(updatedBrightness));
            this->_strip->SetBrightness(updatedBrightness);
        }
    };
    
    this->_brightFadingAnimator->StartAnimation(1, time, brightAnimUpdate);   
}

void NeoControl::StartPulseColorAnimation()
{ 
    
} 


/** private **/

void NeoControl::_setStripColor(RgbColor color)
{
    for (uint16_t pixel=0; pixel<=_countPixels; pixel++)
    {
        this->_strip->SetPixelColor(pixel, color);
    }
    this->_strip->Show();
    Serial.println(_currColor.toString(','));
}


/*void NeoControl::_blendAnimUpdate(const AnimationParam& param)
{ 
    //Serial.println("blendAnimUpdate...");
    RgbColor updatedColor = RgbColor::LinearBlend(
        _animationState[param.index].StartingColor,
        _animationState[param.index].EndingColor,
        param.progress);
    
    for (uint16_t pixel = 0; pixel < _countPixels; pixel++)
    {
        this->_strip->SetPixelColor(pixel, updatedColor);
    }
}

void NeoControl::_pulseAnimUpdate(const AnimationParam& param)
{
    ///Serial.println("PulseAnimUpdate...");
    if (param.state == AnimationState_Completed)
    {
        if (_effectState == 0)
        {
            //Serial.println("effectState = 0");
            uint16_t time = 1000;
            
            _animationState[0].StartingColor = this->_strip->GetPixelColor(0);
            _animationState[0].EndingColor = RgbColor(20,30,5);
    
            this->_waitingAnimator->StartAnimation(1, time, _blendAnimUpdate);
        }
        else if (_effectState == 1)
        {
            //Serial.println("effectState = 1");
            // fade to black
            uint16_t time = 1000;

            _animationState[0].StartingColor = this->_strip->GetPixelColor(0);
            _animationState[0].EndingColor = _currColor;
    
            this->_waitingAnimator->StartAnimation(1, time, _blendAnimUpdate);
        }
        
        _effectState = (_effectState + 1) % 2;
        this->_waitingAnimator->RestartAnimation(param.index);
    }
}*/
