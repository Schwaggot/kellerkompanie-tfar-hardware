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
* over 9000 cables

## Software
Overall there are 3 pieces of software involved. The arduino board does all the stuff related to pressing buttons and writing its current settings to the serial channel where it can be read by the C/C++ ArmA extension (.dll) and finally forwarded to the ingame SQF code adjusting the actual TFAR settings. For development I used:
* arduino IDE
* Visual Studio 2017
* ArmA3 x64 on Windows
* SublimeText 3 (for SQF editing)

### Arduino
For implementing the arduino code I used the default IDE (https://www.arduino.cc/en/Main/Software). The source code can be found inside the arduino folder in the IDE's project format. All additional libraries can be found inside the arduino folder and have been originally downloaded from elegoo (https://www.elegoo.com/download/).

Basically the arduino listens to button presses and stuff and updates the volume/channel/frequency accordingly. The keypad is used to enter frequencies, the \* is used to edit the decimal places and the # switches between additional channel and normal, the remaining A-D keys are all mapped to reset the current frequency. The rotary encoder is used to adjust the volume between 0 and 100. Obviously the LCD displays the channel, frequency and volume. The two buttons are for switching between the channels.

Upon any input on the serial channel the arduino will dump the current settings (volume/channel/frequencies etc.) into the serial output in an array format.

### ArmA 3 extension
For connecting the arduino hardware to ArmA I have written a small extension. The code for it was written in C/C++ in Visual Studio 2017 as described by KK in his blog (http://killzonekid.com/arma-scripting-tutorials-how-to-make-arma-extension-part-1/). In my first version I had the problem that the extension would timeout after 2000ms not doing anything (rpt file showing extension took to long to respond), so I switched to a threaded implementation as described by KK (http://killzonekid.com/arma-scripting-tutorials-how-to-make-arma-extension-part-4/). All in all the extension is getting called by SQF code from inside ArmA (ingame) and then writes something into the serial channel on which the arduino is attached (currently hard coded based on my setup), which then in turn dumps the current information in an array format which is passed on by the ArmA extension to the ingame SQF code. 

### ArmA 3 SQF
The SQF code is based on KK's threaded extension example (http://killzonekid.com/arma-scripting-tutorials-how-to-make-arma-extension-part-4/). Essentially what it does is to poll the extension for information from the arduino and upon receiving the input it parses the array containing the relevant information and setting the TFAR settings accordingly.
