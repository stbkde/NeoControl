# NeoControl

Your LED IoT! Let's light up your TV board.

At this moment, NeoControl only works with WS281X leds. I'll fix that later.

## Features
+ Waiting animation (startup receiver, beamer, ...)

### In progress...
+ Different waiting animations
+ Power saving mode

## Dependancies
+ [NeoPixelBus by stbkde](https://github.com/stbkde/NeoPixelBus) (I made some changes for better color handling. Not needed for NeoControl) forked from [NeoPixelBus by Makuna](https://github.com/Makuna/NeoPixelBus)
+ [Async MQTT client for ESP8266 by Marvin Roger](https://github.com/marvinroger/async-mqtt-client)
  
## Getting Started
```c++
NeoControl StripControl(30, 3); // pixelcount, pin

// add to mainsetup!
StripControl.setup();

// add to mainloop!
StripControl.loop();

StripControl.SetStripColor(RgbColor(123,76,107));
StripControl.SetStripBrightness(198);   // max. 255

// Switch power state of your strip 
StripControl.PowerOn();
StripControl.PowerOff();

// check state    
if (StripControl.PowerState() == ON)   // you can use ON/OFF instead of true/false
{ ... }

// Start waiting animation
StripControl.StartWaitingAnimation();
StripControl.StopWaitingAnimation();
