set path=%path%;C:\ExtraProgramme\Programmierung\WinAVR20070525\bin
avrdude.exe -p T44 -P com3 -c avr910 -U flash:w:"v3_debug.hex":i -U eeprom:w:"v3_debug.eep":i
