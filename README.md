# Kellerkompanie TFAR Hardware
This is a little proof of concept showing how to adjust ingame TFAR settings using a arduino hardware device. Since I haven't touched micro controllers and C/C++ in a very long time I assume that there are a few places experts will find revolting as well as many possibilites for overall improvement. As I believe in the sharing of knowledge you are free to use all of the stuff for your own purpose, of course code taking from already existing projects or other sources may be licensed differently, but I have tried to indicate in which places code was taken from 3rd party sources.

### Video
A small proof of concept video can be found on YouTube: https://youtu.be/Jots-BI9J68

## Hardware
All the parts I used are taken from the elegoo mega 2560 'The most complete starter kit' (https://www.elegoo.com/product/elegoo-mega-2560-project-the-most-complete-starter-kit/):
* 1x arduino mega 2560
* 1x USB cable (to connect the arduino, duhh)
* 1x solder-less breadboard
* 1x potentiometer (10k)
* 1x rotary encoder module
* 1x LCD 1602 module (with pin header)
* 1x membrane switch module (aka keypad)
* 2x button (small)
* 2x 10kOhm resistor
* >9000 jumper cables

## Software
### Arduino
For implementing the arduino code I used the default IDE (https://www.arduino.cc/en/Main/Software). The source code can be found inside the arduino folder in the IDE's project format. All additional libraries can be found inside the arduino folder and have been originally downloaded from elegoo (https://www.elegoo.com/download/).

Basically the arduino listens to button presses and stuff and updates the volume/channel/frequency accordingly. The keypad is used to enter frequencies, the \* is used to edit the decimal places and the # switches between additional channel and normal, the remaining A-D keys are all mapped to reset the current frequency. The rotary encoder is used to adjust the volume between 0 and 100. Obviously the LCD displays the channel, frequency and volume. The two buttons are for switching between the channels.

### ArmA 3 extension
The 

### ArmA 3 sqf
