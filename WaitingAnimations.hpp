#pragma once

#include "NeoAnimator.h"


void FadeToRgbColor(uint16_t time, RgbColor targetColor, NeoPixelBrightnessBus<WS281X_FEATURE, WS281X_METHOD> *bus);
