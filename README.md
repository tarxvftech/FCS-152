# FCS-152

The FCS-152 is a PRC-152 visual clone with simple VHF/UHF FM functionality, roughly equivalent to your standard baofeng, made by FCS.

What sets it apart is the main MCU is an ESP32S2 (with wifi but
not bluetooth) and the source code for the KDU and radio are _open
source_. 

That's where this source code came from. It was not provided
with a license, but it was expressly put out there according to the
marketing materials as seen on aliexpress for 'amateur radio hacking'
so we're assuming it's something fairly open.

Long term I'd like this to get pulled into OpenRTX, but meanwhile we
can still make some improvements.

## Branches

* master - main development branch. Not guaranteed to build.
* release - guaranteed to build and run for some value of 'run'.

## Upstream

[Device information page](http://www.fcs-tactical.com/FCS152A) (you
probably want google translate). We have not seen any Rev1's without
the ESP32 in the wild.

You can purchase one with a good experience from cyanpoem at hotmail,
or px-airsoft.com. Stay away from the aliexpress sellers, they won't
always include everything you need.

Original source code is from the [download
page](http://www.fcs-tactical.com/download) and the link titled '[Open
Source Interface Code](http://www.fcs-tactical.com/OSIC)'. This is also
where you can find the OEM CPS to program channels in.

OEM software, drivers, pdfs, etc all at [the official spec
page](http://www.fcs-tactical.com/FCS152A), just scroll down.

TODO: Offer a mirror of everything translated. FCS did a great job with
the instructions visually, so offering excellent how-to-use documentation
is more about internationalizing the text than anything else.

## Hardware

* Main MCU: ESP33S2-WROOM-I, source code under `test152/`.
* KDU MCU: CM32 which I've never heard of, source code under `KDUCM32/`. I
haven't even looked at this yet.

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

There was an old hardware revision with a non-ESP32 processor. Be
careful that you do not try to flash firmware for this revision to the
new hardware and vice-versa. The new hardware is sometimes referred to as `-N` or as `152+`. The old hardware is often referred to in various flavors of version 1.0 or `152(A)`.


#### Buttons, inputs:

The front keyboard matrix is scanned by `Matrix_KEY_Scan()`.
The top encoder button press (and only the press) is handled by
`Encoder_Switch_Scan()`. The side buttons, `VOL_ADD`, `VOL_SUB`, `PTT`,
and `SQUELCH` are manually read.

The encoder is read by an interrupt. It works, but could be improved -
but to improve it will require the menu system and input handling be
basically completely replaced. That'll take a while.

## Basic instructions and gotchas

Power on the FCS-152 by pressing in and holding the top button. Same to
turn it off. Audio must be explicitly configured in the PGM menu, it
doesn't auto switch to accessories like headsets.


## Docs

[Internal pictures](https://tarxvf.tech/blog/20221105_fcs-152/)
See also further in this file for documentation derived from the source code.

## Building the firmware

This is not documented anywhere and not known outside of FCS yet. They
note that the arduino support package needs some modifications of some
kind, but that's not documented anywhere. It may be best to look at
this source code release as a way of documenting how the hardware is
configured and how the firmware works without actually providing a
buildable firmware. An [OpenRTX](https://openrtx.org/) port to this
platform seems like an excellent idea.


ESP32S2 support seems to only be after esp32 ver 2.0 in Boards Manager.

## Communicating with the radio

To boot into the 'second system' as the source code calls it, hold the
button under the PTT during boot. This will bring you to a menu that
shows options to enter CPS Read/Write mode, a wifi/http based firmware
upgrade mode, and a version readout.

To enter the ESP32S2 bootloader, hold the number 9 on the keypad and hold
the power button (push down on the encoder knob).  You can let go of the
9, but you can't let go of the power button until the flash is complete.

Yes, we know.

This was determined by experiment and realizing GPIO-0 is row 3 on the
keypad, plus the lovely minicom project pointed at the USB serial port
to let us know when something interesting happened.

While you hold that power button, you can initiate a successful, standard
arduino flash from speeds of 115200 up to 460800 baud. Possibly more
but 460800 was the fastest reliable speed I could get.  Faster is better
since you have to hold the power button down the entire time.

We don't yet know the optimal settings for the project but the radio
still appears to run, so that's nice.



## Documentation derived from the source code:

UART 1 is side connector at 115200 baud -- KDU seems to use this and maybe other things

UART 2 is radio module configuration at 9600 baud, uses AT commands  

AT Commands to setup the radio -- copied direct from source:

    - "AT+DMOSETGROUP=1,436.025,436.025,000,1,001,1\r\n"
    - "AT+DMOSETMIC=1,0,0\r\n"
    - "AT+DMOREADRSSI\r\n"
    - "AT+DMOSETVOLUME=5\r\n"
    - 
    AT Command success responses:
    - "+DMOCONNECT:0" - succcesful connection
    - "+DMOSETGROUP:0" - successful "write"
    - "+DMOSETMIC:0" -- maybe mic sensitivity?  
    - "+DMOAUTOPOWCONTR:0" -- automatic power control
    - "+ DMOSETVOLUME:0" -- probably a typo with the space after the plus sign but successful volume set 


    microcontroller Pins (Arduino pin numbers):
    36 - LCD CS 
    37 - LCD RST 
    38 - LCD RS 
    39 - LCD SDA 
    40 - LCD SCL 

    13 - M62364_LD_Pin
    12 - M62364_CLK_Pin
    11 - M62364_DATA_Pin

    5 - VDO 
    6 - Mic in En 
    7 - Mic out en 
    1 - SPK in En 
    0 - SPK Out En 


    gpio numbers 
    gpio 10 - ADC
    gpio 18 - DAC 
    gpio 35 - PWM 
    gpio 25 - wakeup (also Encoder Click)
    gpio 6 - volume up
    gpio 7 - volume down 
    gpio 4 - PTT 
    gpio 5 - squelch

    gpio 20 - encoder left spin
    gpio 19 - encoder right spin

    gpio 33 - power enable 

    gpio 15 - A002 Squelch pin 
    gpio 16 - A002 PD Pin 
    gpio 14 - A002 PTT Pin

    gpio 2 - Key Row 1 
    gpio 3 - Key Row 2
    gpio 0 - Key Row 3
    gpio 1 - Key Row 4 
    gpio 41 - Key Col 1 
    gpio 42 - Key Col 2
    gpio 45 - Key Col 4
    

    UART1 TX = GPIO10, pin 29 , IO10
    UART1 RX = GPIO9, pin 28 ,  IO9

    UART2 TX = GPIO17, Pin 27 , IO17
    UART2 RX = GPIO16, Pin 25 , IO16

    UART0 TX = GPIO1, Pin 41 , 
    UART0 RX = GPIO3, Pin 42 , 

    USB D+ GPIO20, pin 20 (Note the same assignment as the encoder gpios)
    USB D- GPIO19, Pin 19











## Problems with dreams

At least right now, it doesn't look like the ESP32 has any way to sample
the baseband. It also doesn't appear able to drive the speakers/headphones
or sample the microphone directly. There is definitely some kind of
way that it can send tones through the transmission, but that's not
characterized yet.

In short, it's probably stuck as what it is for now. Maybe there can be
some improvements to allow it to do other things with a hardware revision.


## Dreams - probably really only for future hardware revisions

Radios - it's easy to forget this has two radios. You can do short-range
very-high-bandwidth encrypted communications very easily using the
ESP32 itself.

The radio module uses serial, and could be trivially replaced with an
MMDVM module and then you can have a multi-protocol data radio, or P25
and M17 with relatively little effort. Getting more power out than
a hotspot will require new hardware, but a PoC with a hotspot modem
could be done on the hardware side in a night.

Similarly the RF board could be replaced with basically any RF board you
like and you can finally have a some low-VHF or other non 2m/70cm boards.

None of this matters much if there's no ability to read the microphone
and drive the speaker from the ESP32.

For that matter you can put a 2.4GHz antenna on the antenna port and
find a way to extend the main coax down to the ESP32 and disable the RF
board entirely, or put a 2.4ghz amplifier in place.
* https://github.com/Jeija/esp32free80211
* https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/esp-wifi-mesh.html
* https://github.com/espressif/esp-idf/blob/master/docs/en/api-guides/wifi.rst#wi-fi-80211-packet-send

If the baseband is unfiltered and the ESP32 samples it directly, we
might be able to bring M17 to the radio natively without hardware
changes. I wouldn't get my hopes up, but this is on the list of things
to test.

If I understand correctly, the ESP32S2 has native USB. It might be
possible to provide sane CPS programming (drag and drop files, ...),
ethernet over USB for packet, etc. Unfortunately, it seems the encoder
is connected to the USB pins, so this won't be happening on this hardware.

Similarly it has wifi. Connecting with ATAK should be a cinch once it
can do data.

GPS could be provided over wifi or serial on the side port. Maybe provided
from a connected mobile phone. Once we map all the pins we may find enough
to shoehorn a GPS in. APRS transmission seems possible since the ESP32 is
generating beeps that get transmitted, but APRS RX has yet to be proven.
