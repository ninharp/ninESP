---
title: flashing
---
# Flashing to your board

If you just want to flash the firmware without the compiling, you can take one of our binary releases from the firmware folder in the repository. 
There are zip archives with the firmware files in it. 
Its not one file builds so you have to flash it seperately. The flash addresses are the file names of the binaries.
Except of the spiff rom, which had to be in this case on memory address *0x5A000*. 
An example flashing command with esptool could look like this.

```esptool.py -p /dev/ttyUSB0 -b 230400 write_flash -ff 40m -fm qio -fs 32m 0x00000 0x00000.bin 0xa000 0xa000.bin 0x5A000 spiff_rom.bin```