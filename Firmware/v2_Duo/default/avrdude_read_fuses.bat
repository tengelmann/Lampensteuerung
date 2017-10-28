echo off
echo Takt auf 8Mhz
echo Brown-Out-Detection enabled 4.3V
avrdude.exe -p T44 -P com1 -c avr910 -v

