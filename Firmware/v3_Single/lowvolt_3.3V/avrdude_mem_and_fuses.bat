set path=%path%;C:\ExtraProgramme\Programmierung\WinAVR-20070525\bin
avrdude.exe -p T44 -P com3 -c avr910 -U flash:w:"v3_Single.hex":i -U eeprom:w:"v3_Single.eep":i
avrdude.exe -p T44 -P com3 -c avr910 -U lfuse:w:0xe2:m -U hfuse:w:0xdd:m