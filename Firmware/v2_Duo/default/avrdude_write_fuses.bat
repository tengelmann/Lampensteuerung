echo off
echo Takt auf 8Mhz
echo Brown-Out-Detection enabled 2,7V 
echo e2 dc ff
avrdude.exe -p T44 -P com3 -c avr910 -U lfuse:w:0xe2:m -U hfuse:w:0xdd:m

