
compile: pyclean
	arduino-cli compile -b esp32:esp32:esp32s2
pyclean:
	#clean up anything from python font/img tools
	-rm *.o *.so _font.*

font.so: font.cpp font.h
	g++ -c -Wno-narrowing font.cpp -o font.o
	g++ -shared -o font.so font.o
flash:
	echo "hold 9 on the keypad, press and hold power on radio. You can release the 9 but must hold the power (encoder press) the entire time radio is flashing"
	arduino-cli upload -b esp32:esp32:esp32s2:UploadSpeed=460800  -p /dev/ttyUSB0
flash2:
	arduino-cli upload -b esp32:esp32:esp32s2:UploadSpeed=460800,EraseFlash=all  -p /dev/ttyUSB0

flashweb1:
	cp /tmp/arduino-sketch-*/test152.ino.bin test152.ino.bin
	#curl 'http://esp32s2-D7594A/_ac/update_act' -X POST -F "bin=@test152.ino.bin" -o/dev/null
	curl 'http://FCS152-4A59D/_ac/update_act' -X POST -F "bin=@test152.ino.bin" -o/dev/null
	rm test152.ino.bin
flashweb2:
	cp /tmp/arduino-sketch-*/test152.ino.bin test152.ino.bin
	curl 'http://FCS152-7EDFD/_ac/update_act' -X POST -F "bin=@test152.ino.bin" -o/dev/null
	#curl 'http://esp32s2-D7DF7E/_ac/update_act' -X POST -F "bin=@test152.ino.bin" -o/dev/null
	rm test152.ino.bin
