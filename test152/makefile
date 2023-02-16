

compile:
	arduino-cli compile -b esp32:esp32:esp32s2
flash:
	echo "hold 9 on the keypad, press and hold power on radio. You can release the 9 but must hold the power (encoder press) the entire time radio is flashing"
	arduino-cli upload -b esp32:esp32:esp32s2:UploadSpeed=460800  -p /dev/ttyUSB0

