@ECHO OFF
"C:\ExtraProgramme\Programmierung\AVRTools\AvrAssembler2\avrasm2.exe" -S "D:\Projekte\Fahrradlampe\Digitale Lampensteuerung\Software\AVRootloader\AVR\labels.tmp" -fI -W+ie -C V2 -o "D:\Projekte\Fahrradlampe\Digitale Lampensteuerung\Software\AVRootloader\AVR\AVRootloader.hex" -d "D:\Projekte\Fahrradlampe\Digitale Lampensteuerung\Software\AVRootloader\AVR\AVRootloader.obj" -e "D:\Projekte\Fahrradlampe\Digitale Lampensteuerung\Software\AVRootloader\AVR\AVRootloader.eep" -w -m "D:\Projekte\Fahrradlampe\Digitale Lampensteuerung\Software\AVRootloader\AVR\AVRootloader.map" -l "D:\Projekte\Fahrradlampe\Digitale Lampensteuerung\Software\AVRootloader\AVR\AVRootloader.lst" "D:\Projekte\Fahrradlampe\Digitale Lampensteuerung\Software\AVRootloader\AVR\AVRootloader.asm"