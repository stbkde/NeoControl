# NeoControl

Let's light up your TV board.

## Features
+ Waiting animation (startup receiver, beamer, ...)

### In progress...
+ Different waiting animations

## Dependancies
+ [NeoPixelBus by stbkde](https://github.com/stbkde/NeoPixelBus) (I made a few changes for better color handling) forked from [NeoPixelBus by Makuna](https://github.com/Makuna/NeoPixelBus). Many thanks [Makuna](https://github.com/Makuna) for the great library
  
## Getting Started
At the moment by default NeoControl only works with WS281X leds. I'll fix that later.

```c++
#include <NeoControl.h>

NeoControl StripControl(30, 3); // pixelcount, pin

// add to mainsetup!
StripControl.setup();

// add to mainloop!
StripControl.loop();

// set the strip color as HslColor
StripControl.SetStripColor(HslColor(230,75,65));

// you can set the color as Rgb and Hsb too
HsbColor hsbcol(240,65,75);

String rgbval = "155,25,210";
RgbColor rgbcol(rgbval, ',');

StripControl.SetStripColor(HslColor(hsbcol));
StripControl.SetStripColor(HslColor(rgbcol));
```
### Brightness

The brightness can be controlled via the luminance value of the HslColor, Range [0, 100]
```c++
HslColor hslcol(230,75,65);
targetBrightness = 60;  // range [0, 100]
HslColor newColor(hslcol.H, hslcol.S, float(targetBrightness/100));
StripControl.SetStripColor(newColor);
```
### Toogle the power state of your strip 
```c++
StripControl.PowerOn();
StripControl.PowerOff();

// check state    
if (StripControl.PowerState() == ON)   // you can use ON/OFF instead of true/false
{ ... }
```
### Waiting animation
```c++
StripControl.StartWaitingAnimation();
StripControl.StopWaitingAnimation();
```
