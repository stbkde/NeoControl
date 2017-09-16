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
NeoControl StripControl(30, 3); // pixelcount, pin

// add to mainsetup!
StripControl.setup();

// add to mainloop!
StripControl.loop();

// fade to color
StripControl.SetStripColor(RgbColor(123,76,107));

// fade to brightness
StripControl.SetStripBrightness(198);   // max. 255

// toogle power state of your strip 
StripControl.PowerOn();
StripControl.PowerOff();

// check state    
if (StripControl.PowerState() == ON)   // you can use ON/OFF instead of true/false
{ ... }

// Start waiting animation
StripControl.StartWaitingAnimation();
StripControl.StopWaitingAnimation();
