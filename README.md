
#RC-SITL#


Program that allows the use of an RC Transmitter with ArduPilot SITL
    -The program relies on DroneApi
    -Sends RC signals over MAVlink at 50hz

Daniel Nugent
with DroneApi snippets from Randy Mackay


###Hardware:###
-Attach a PPM signal(from trainer port on transmitter or PPM out on Receiver) into pin 8 of Arduino
-Attach GND from signal source into GND of Arduino
-Attach the Arduino over USB.


###To use:###
1. load program on Arduino and setup up Serial USB connection
2. Start SITL
3. load DroneAPI
4. api start [PATH]/RCControl.py



This program isn't very robust and does not handle errors well
###Potential areas of trouble:###
Serial port not found -> check to make sure you are connected to the right port
Serial port busy -> two RCControl threads trying to use the same port. Restart the simulator and RCControl.py
Serial port gibberish: reads the stream wrong, almost like baudrate mismatch -> check baudrate or restart computer
Parse serial stream wrong -> exception is thrown and should be caught but it might not be...
null(0) rc override commands received by MAVlink -> power cycle transmitter/check wiring/reset arduino

This program may chew up mavlink bandwidth but it was a simple hack in an attempt to obtain RC control in SITL
There is probably a better way to implement this but this was just a quick tool
It should be nonblocking(not throw exceptions when it fails) and will just print a message if it doesn't start
    -but it may, haven't fully tested it.

I start RCControl.py using .mavinit.scr

