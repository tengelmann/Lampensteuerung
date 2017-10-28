@echo off
set pathtoavrdude=C:\ExtraProgramme\Programmierung\AvrDude5.1\
set com=com2
set project=v3_Single

%pathtoavrdude%avrdude.exe -p T44 -P %com" -b 115200 -c avr910 -U flash:w:"%project%.hex":i -U eeprom:w:"%project%.eep":i -U lfuse:w:0xe2:m -U hfuse:w:0xdd:m

pause