@echo off
set com=com1


set hex="AVRootloader.hex"

set efuse=0xfe
set hfuse=0xdd
set lfuse=0xe2


avrdude.exe -p T84 -P %com% -b 115200 -c avr910 -U flash:w:%hex%:i -U lfuse:w:%lfuse%:m -U hfuse:w:%hfuse%:m -U efuse:w:%efuse%:m

pause