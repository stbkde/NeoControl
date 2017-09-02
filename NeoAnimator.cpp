/*-------------------------------------------------------------------------
NeoAnimator provides animation timing support.

Written by Stephan Beier.

I invest time and resources providing this open source code,
please support me by dontating (see https://github.com/Makuna/NeoPixelBus)

-------------------------------------------------------------------------
NeoPixelBus is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

NeoPixelBus is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with NeoPixel.  If not, see
<http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/

#include "NeoAnimator.h"

NeoAnimator::NeoAnimator(uint16_t countAnimations, uint16_t timeScale) :
    _countAnimations(countAnimations),
    _animationLastTick(0),
    _activeAnimations(0),
    _isRunning(true)
{
    setTimeScale(timeScale);
    _animations = new AnimationContextEx[_countAnimations];
}

NeoAnimator::~NeoAnimator()
{
    delete[] _animations;
}

bool NeoAnimator::NextAvailableAnimation(uint16_t* indexAvailable, uint16_t indexStart)
{
    if (indexStart >= _countAnimations)
    {
        // last one
        indexStart = _countAnimations - 1;
    }

    uint16_t next = indexStart;

    do
    {
        if (!IsAnimationActive(next))
        {
            if (indexAvailable)
            {
                *indexAvailable = next;
            }
            return true;
        }
        next = (next + 1) % _countAnimations;
    } while (next != indexStart);
    return false;
}

void NeoAnimator::StartAnimation(NeoPixelBrightnessBus<WS281X_FEATURE, WS281X_METHOD> *bus,
                                 uint16_t indexAnimation,
                                 uint16_t duration, 
                                 AnimUpdateCallbackEx animUpdate)
{
    if (indexAnimation >= _countAnimations || animUpdate == NULL)
    {
        return;
    }

    if (_activeAnimations == 0)
    {
        _animationLastTick = millis();
    }

    StopAnimation(indexAnimation);

    // all animations must have at least non zero duration, otherwise
    // they are considered stopped
    if (duration == 0)
    {
        duration = 1;
    }

    _animations[indexAnimation].StartAnimation(*&bus, duration, animUpdate);

    _activeAnimations++;
}

void NeoAnimator::StopAnimation(uint16_t indexAnimation)
{
    if (indexAnimation >= _countAnimations)
    {
        return;
    }

    if (IsAnimationActive(indexAnimation))
    {
        _activeAnimations--;
        _animations[indexAnimation].StopAnimation();
    }
}

void NeoAnimator::StopAll()
{
    for (uint16_t indexAnimation = 0; indexAnimation < _countAnimations; ++indexAnimation)
    {
        _animations[indexAnimation].StopAnimation();
    }
    _activeAnimations = 0;
}


void NeoAnimator::UpdateAnimations()
{
    if (_isRunning)
    {
        uint32_t currentTick = millis();
        uint32_t delta = currentTick - _animationLastTick;
        
        if (delta >= _timeScale)
        {
            AnimationContextEx* pAnim;

            delta /= _timeScale; // scale delta into animation time

            for (uint16_t iAnim = 0; iAnim < _countAnimations; iAnim++)
            {
                pAnim = &_animations[iAnim];
                AnimUpdateCallbackEx fnUpdate = pAnim->_fnCallback;
                AnimationParamEx param;
                
                param.index = iAnim;

                if (pAnim->_remaining > delta)
                {
                    param.state = (pAnim->_remaining == pAnim->_duration) ? Animation_State_Started : Animation_State_Progress;
                    param.progress = (float)(pAnim->_duration - pAnim->_remaining) / (float)pAnim->_duration;
                    param.pixelbus = pAnim->_pixelBus;
                    
                    fnUpdate(param);

                    pAnim->_remaining -= delta;
                }
                else if (pAnim->_remaining > 0)
                {
                    param.state = Animation_State_Completed;
                    param.progress = 1.0f;
                    param.pixelbus = pAnim->_pixelBus;

                    _activeAnimations--; 
                    pAnim->StopAnimation();

                    fnUpdate(param);
                }
            }

            _animationLastTick = currentTick;
        }
    }
}
