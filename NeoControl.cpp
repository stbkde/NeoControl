#include "NeoControl.h" 
    
NeoControl::NeoControl()
{
    this->_strip = new NeoPixelBrightnessBus<WS281X_FEATURE, WS281X_METHOD>(WS281X_STRIP_COUNT, WS281X_STRIP_PIN);
    
    _pin            = WS281X_STRIP_PIN;
    _countPixels    = WS281X_STRIP_COUNT;
    
    this->_waitingAnimator      = new NeoPixelAnimator(_countPixels);
    this->_brightFadingAnimator = new NeoPixelAnimator(_countPixels);
    this->_colorFadingAnimator  = new NeoPixelAnimator(_countPixels);
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
        Serial.println("NEW COLOR and waiting animator IS animating");
        
        this->_colorFadingAnimator->StopAll();
        
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
        Serial.println("NEW COLOR and waiting animator is NOT animating");
        this->_colorFadingAnimator->StopAll();
        
        if (_powerState == ON)
        {
            Serial.println("NEW COLOR _powerState is ON");
            _lastColor = _currColor;
            _currColor = color;
            
            //_setStripColor(color);
            FadeToRgbColor(_colorFadingTime, color);
        }
        else 
        {
            Serial.println("NEW COLOR _powerState is OFF");
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
                
                this->_strip->SetBrightness(targetBrightness);
                //FadeToBrightness(_brightnessFadingTime, targetBrightness);
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
                
                this->_strip->SetBrightness(targetBrightness);
                //FadeToBrightness(_brightnessFadingTime, targetBrightness);
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
            _lastBrightness = 100;
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