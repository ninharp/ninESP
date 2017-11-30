## Local build configuration
## Parameters configured here will override default and ENV values.

## Add your source directories here separated by space
# MODULES = app
EXTRA_INCDIR = include

## ESP_HOME sets the path where ESP tools and SDK are located.
ESP_HOME = /opt/esp-open-sdk

## SMING_HOME sets the path where Sming framework is located.
SMING_HOME = /opt/sming/Sming

## COM port parameter is reqruied to flash firmware correctly.
COM_PORT = /dev/ttyUSB0

## Com port speed
COM_SPEED	= 230400

## Configure flash parameters (for ESP12-E and other new boards):
#SPI_MODE = dio

## SPIFFS options
# DISABLE_SPIFFS = 1
SPIFF_FILES = web/build

