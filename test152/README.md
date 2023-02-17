## Bugs fixed:

* Frequency input not allowing certain digits to be typed. Removed per-character entry resstrictions entirely.

* non-volatile store `set_flag` was getting incorrect values due to
THISCHIP being `THISCHIP_ESP32S2` instead of `ESP32S2`, meaning it was
indexing position 137 of a 3 element array because it was using the
non-esp32 EEPROM preprocessor defines instead of a 3-value FLAG enum as
expected. This resulted in a very interesting bug where _removing code_
made the radio fail to boot due to a flash storage issue, despite that
code being _after_ the crash and not executing.
