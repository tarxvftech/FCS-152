## Bugs fixed:

* Frequency input not allowing certain digits to be typed. Removed per-character entry resstrictions entirely.

* non-volatile store `set_flag` was getting incorrect values due to
THISCHIP being `THISCHIP_ESP32S2` instead of `ESP32S2`, meaning it was
indexing position 137 of a 3 element array because it was using the
non-esp32 EEPROM preprocessor defines instead of a 3-value FLAG enum as
expected. This resulted in a very interesting bug where _removing code_
made the radio fail to boot due to a flash storage issue, despite that
code being _after_ the crash and not executing.


## Features added, or other improvements
### Of interest to users:
### Of interest to devs:
* Wifi softAP, captive portal, STA mode with multiple SSID support, web server, local http firmware upgrade


## Libraries
AutoConnect

# Code map
## Serial ports (code references, not necessarily hardware names)
* Serial / UART1 -> side connector 
* Serial1 / UART2 -> a002 FM transceiver module control

# Where to start

`FCS152_KDU.h` has global preprocessor defines and such.

`test152.ino` is where you'll find the arduino-standard setup() and loop().

`main_fun.cpp` is a giant pile of code where you'll find the majority of the UI menu, and input handling.

Most other files are fairly well self-describing, though it looks and
feels like there were two or more developers working on it - some code
is very clean and well-organized, and most of the glue code is a little
rough-and-dirty. Maybe someone wrote drivers and someone else wrote the
main app, or maybe someone ran out of time and had to hurry to get it
working and shipped (we've all been there!).

Regardless, thank you very much to FCS for open sourcing this code,
allowing it to be understood, improved, and built-upon. That's very rare
in the radio space, and it's why the FCS-152 is my favorite radio.
