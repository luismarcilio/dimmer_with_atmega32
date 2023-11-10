# dimmer_with_atmega32

This is the code used in my bluetooth dimmer with a bluetooth dongle.
At that time, home automation was giving his first steps. There was no Alexa or Apple Home.

## Hardware

The circuit has the high voltage part isolated from the low voltage part by an optocoupler.

The hardware wasn't very difficult to develop. It's a Triac triggered by a pulse from the uC. 

There is a zero passing circuit that triggers an interruption that starts a counter in the uC when the voltage passes through zero.
When the counter reaches its target, it sends a high pulse to one of the uC terminals that triggers the Triac gate to let the current flow.
The counter target is controlled by Bluetooth software.

## Software

A bluetooth dongle doesn't implement much of the bluetooth protocol stack. 
We had to implement three of the main blutooth protocols:
* HCI
* L2CAP
* HID



  

  
