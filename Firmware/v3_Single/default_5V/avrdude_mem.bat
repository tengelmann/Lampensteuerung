@set pathtoavrdude=C:\ExtraProgramme\Programmierung\AvrDude5.1\
@%pathtoavrdude%avrdude.exe -P com2 -b 115200 -p T44 -c avr911 -U flash:w:"v3_Single.hex":i -U eeprom:w:"v3_Single.eep":i
@pause
