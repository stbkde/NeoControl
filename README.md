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
```
### The power state
Toogle the power state of your strip
```c++
StripControl.PowerOn();
StripControl.PowerOff();

// check state    
if (StripControl.PowerState() == ON)   // you can use ON/OFF instead of true/false
{ ... }
```
### Color
Set the color as HslColor.
```c++
// HSL color
StripControl.SetStripColor(HslColor(230,75,65));

HsbColor hsbcol(240,65,75);
StripControl.SetStripColor(HslColor(hsbcol));

// RGB color
// parse it directly from a string
String rgbval = "155,25,210";
RgbColor rgbcol(rgbval, ',');

StripControl.SetStripColor(HslColor(rgbcol));

// convert between color spaces
HslColor hsl(HsbColor(230,70,60));
RgbColor rgb(StripControl.GetCurrentColor());   // hsl -> rgb
```
### Brightness (Luminance)

The brightness can be controlled via the luminance value of the HslColor, Range [0, 100]
By default the maximum brightness (luminance) is at 80 and the minimum is 20.
```c++
HslColor hslcol(230,75,65);
int targetBrightness = 60;  // range [0, 100]
HslColor newColor(hslcol.H, hslcol.S, float(targetBrightness/100));
StripControl.SetStripColor(newColor);
```
### Waiting animation
```c++
StripControl.StartWaitingAnimation();
StripControl.StopWaitingAnimation();
```
