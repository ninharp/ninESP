---
title: How to compile
tag: howtocompile
---

# Step by Step: Compiling


## 1. Install SDK

Install the integrated SDK for ESP8266/ESP8285 chips from this repository [esp-open-sdk](https://github.com/pfalcon/esp-open-sdk) 
after that make sure that your current shell has the xtensa-lx106 compiler in PATH.

## 2. Clone ninESP Repository

Open a Terminal and change to a directory of your choice where to clone the repository.

```shell
$ git clone --recurse-submodules git@github.com:ninharp/ninESP.git ninESP

# Change to repository directory
$ cd ninESP
```
## 3. Prepare the Sming Framework

After successfull cloning of the repository with all submodules we had to update the Sming Framework (we use the develop branch at the moment)

Run the following commands in the repository directory

```sh
# Change to Sming Framework directory
$ cd Sming/Sming 

# Checkout to develop branch of Sming
$ git checkout develop --recurse-submodules

# Update Sming submodules
$ git submodule update --init --recursive

# Do a dist clean on the Sming repository
$ make dist-clean

# Change back to ninESP
$ cd ../..
```

## 4. Patch the MD Libraries
```bash
$ ./patch-linux.sh 
Patching MD_MAX72xx...
patching file MD_MAX72XX/src/MD_MAX72xx.cpp
Patching MD_Parola...
patching file MD_Parola/src/MD_Parola.h
All patching done
```

## 5. Edit Library locations

Edit the *Makefile-user.mk* with an editor of your choice to suit your installation folders. Also you can change the flashing parameters.

**ESP_HOME** had to set to the correct location of your esp-open-sdk installation

**SMING_HOME** should be set to ninESP repository location plus the "Sming/Sming" folder at the end.

_f.E. SMING_HOME=/home/michael/ninESP/Sming/Sming_

```
## Local build configuration
## Parameters configured here will override default and ENV values.

## Add your source directories here separated by space
MODULES = app MD_Parola/src MD_MAX72XX/src
EXTRA_INCDIR = include MD_Parola/src MD_MAX72XX/src

## ESP_HOME sets the path where ESP tools and SDK are located.
ESP_HOME = /opt/esp-open-sdk

## SMING_HOME sets the path where Sming framework is located.
SMING_HOME = /opt/Sming/Sming

## COM port parameter is reqruied to flash firmware correctly.
COM_PORT = /dev/ttyUSB0

[...]
```

## 6. Compile ninESP

To finally compile ninESP you just have to run a _make all_ command in the ninESP folder.
This may take a while...

```
# Just to make sure do a clean
$ make clean

# Compile
$ make all
```

## 7. Flash Firmware

When you flash the ninESP for the first time it is recommended that you erase the whole memory of the ESP8266 initially, you can do this by a _make flashinit_ call. 
Just to make sure you have no saved wifi networks or values stored in flash.

After this you can flash the newly compiled firmware with a simple _make flash_.
Make sure that your COM_PORT is pointing to the correct serial device.

If you get an error on the end of flashing about "Inappropriate Device rights", 
just ignore and use any other serial terminal, f.E. minicom or cutecom.

```
# Erase the ESP8266 flash
$ make flashinit

# Flash the firmware
$ make flash
```
