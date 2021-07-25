#!/bin/sh
## Local build configuration
## Parameters configured here will override default and ENV values.

## Add your source directories here separated by space
MODULES = app MD_Parola/src MD_MAX72XX/src
EXTRA_INCDIR = include MD_Parola/src MD_MAX72XX/src

## ESP_HOME sets the path where ESP tools and SDK are located.
ESP_HOME = /opt/esp-open-sdk

## SMING_HOME sets the path where Sming framework is located.
SMING_HOME = Sming/Sming

## COM port parameter is reqruied to flash firmware correctly.
COM_PORT = /dev/ttyUSB0
