# FCS-152

The FCS-152 is a PRC-152 visual clone with simple VHF/UHF FM functionality, roughly equivalent to your standard baofeng, made by FCS.

What sets it apart is the main MCU is an ESP32S2 (with wifi but
not bluetooth) and the source code for the KDU and radio are _open
source_. 

That's where this source code came from. It was not provided
with a license, but it was expressly put out there according to the
marketing materials as seen on aliexpress for 'amateur radio hacking'.

TODO: provide the links to the device, purchasing information (cyanpoem),
and source code download.


## Hardware

* Main MCU: ESP33S2-WROOM-I, source code under `test152/`.
* KDU MCU: STM32 of some kind, source code under `KDUCM32/`. I haven't
even looked at this yet.

The actual RF chip is a separate module labeled 'A002-U-V' driven by
a GD32F101 (STM32F101 clone) in an oddball footprint. It is configured
over serial using AT commands, and appears to have analog baseband
signals for transmit and receive audio along with PTT and VOX lines,
among other things.

The KDU is powered from the main radio and communicates over serial.

The programming cables for the radio are included with the KDU - you
have to purchase the KDU to get them. Be careful that they are actually
included before you purchase, as at least one aliexpress seller did not
include them.

The HMI (front panel and display) appear to be driven directly by
the ESP32.

## Basic instructions and gotchas

Power on the FCS-152 by pressing in and holding the top button. Same to
turn it off. Audio must be explicitly configured in the PGM menu, it
doesn't auto switch to accessories like headsets.


## Docs

[Internal pictures](https://tarxvf.tech/blog/20221105_fcs-152/)
