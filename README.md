# Water Guardian #

## Swim Unit ##

<img src="https://github.com/brettawaytoday/Water-Guardian/blob/main/Images/WG_swimUnit002.png" height="200"> <img src="https://github.com/brettawaytoday/Water-Guardian/blob/main/Images/WG_swimUnit003.png" height="200">

Water Guardian is made up of a micro controller, Lora Module, GPS module, temperature sensor and rechargeable battery. It is housed in a waterproof sleeve attached to the swimmers rash vest, between their shoulder blades. 
The air pocket at the top end of the sleeve ensures that the antennas will float on the surface and only submerge if the swimmers head becomes submerged.
The microcontroller sends an encrypted “heartbeat” every 2 seconds with time, GPS position and temperature on the LoRa 915MHz band.

<img src="https://github.com/brettawaytoday/Water-Guardian/blob/main/Images/WG_conceptPrototype.png" height="600"> <img src="https://github.com/brettawaytoday/Water-Guardian/blob/main/Images/WG_swimUnit001.png" height="600"> <img src="https://github.com/brettawaytoday/Water-Guardian/blob/main/Images/WG_swimUnit004.png" height="600">

## Base Station ##

A base station on the shore, made up of a micro controller, Lora Module, GPS module and GSM module, powered withs a mains power supply and backed up with an uninterruptible power supply.
This unit receives and logs all “heartbeats” from all the Water Guardians transmitting position and keeps a tally on each one. 
If a heartbeat from a particular device has not been received for more than 10 seconds (5 heartbeats) it will change the status of the device to “Alert status”. 
If a further 20 seconds go by without a heartbeat, the lifeguards will be notified of the last known position of the swimmer.

## Software ##

Software for the platform will be open sourced for transparency and written in a combination of C and microPython for the micro controller. 
A mobile application will also be developed for surf lifesaver monitoring, social monitoring and data collection.
The base station can operate standalone or with the companion app.

## Prototype and Testing ##

On a cold winters morning in July, the first working prototype of Water Guardian was tested in Maraetai. The purpose of the test was to validate the ability for the water unit to reliably communicate with the base station.

The test exceeded all expectations and the water unit was still sending a strong signal from 100m.

The feedback from the swimmer was incredibly positive and there have been a number of interested and willing participants come forward to help with future testing.

While the form factor of the prototype has evolved in time, it has had to accommodate the off the shelf electronics that were used and therefore not the most ideal. 

Ultimately the electronics will be custom built, allowing for a more ergonomic design for the swim unit. 

