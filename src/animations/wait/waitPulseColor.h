/*-------------------------------------------------------------------------
NeoControl

Written by Stephan Beier

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
#pragma once


#include <NeoPixelBrightnessBus.h>
#include <NeoPixelAnimator.h>

#include "../../TWaitAnimation.h"
#include "../../config.h"
#include "../../NeoBus.h"


class waitPulseColor : public TWaitAnimation
{
public:    
    waitPulseColor()
    {
    }
    
    void Start();
};