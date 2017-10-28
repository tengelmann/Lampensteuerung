@set path=%path%;C:\ExtraProgramme\Programmierung\AvrDude
avrdude.exe -p T44 -P com2 -c avr910 -b 115200 -U flash:w:"v3_Single.hex":i -U eeprom:w:"v3_Single.eep":i
