/**
 * Project: ninHOME_Node
 * @file ota.cpp
 * @author Michael Sauer <sauer.uetersen@gmail.com>
 * @date 30.11.2017
 *
 * OTA Firmware update functions
 *
 */
#include "application.h"
#include <ota.h>

#define ROM_0_URL  "http://192.168.8.100:80/rom0.bin"
#define ROM_1_URL  "http://192.168.8.100:80/rom1.bin"
#define SPIFFS_URL "http://192.168.8.100:80/spiff_rom.bin"

rBootHttpUpdate* otaUpdater = 0;

void otaUpdateCb(rBootHttpUpdate& client, bool result)
{

	Serial.println("In callback...");
	if(result == true) {
		// success
		uint8 slot;
		slot = rboot_get_current_rom();
		if (slot == 0) slot = 1; else slot = 0;
		// set to boot new rom and then reboot
		Serial.printf("Firmware updated, rebooting to rom %d...\r\n", slot);
		rboot_set_current_rom(slot);
		System.restart();
	} else {
		// fail
		Serial.println("Firmware update failed!");
	}
}

void otaUpdate()
{

	uint8 slot;
	rboot_config bootconf;

	Serial.println("Updating...");

	// need a clean object, otherwise if run before and failed will not run again
	if (otaUpdater) delete otaUpdater;
	otaUpdater = new rBootHttpUpdate();

	// select rom slot to flash
	bootconf = rboot_get_config();
	slot = bootconf.current_rom;
	if (slot == 0) slot = 1; else slot = 0;

#ifndef RBOOT_TWO_ROMS
	// flash rom to position indicated in the rBoot config rom table
	otaUpdater->addItem(bootconf.roms[slot], ROM_0_URL);
#else
	// flash appropriate rom
	if (slot == 0) {
		otaUpdater->addItem(bootconf.roms[slot], ROM_0_URL);
	} else {
		otaUpdater->addItem(bootconf.roms[slot], ROM_1_URL);
	}
#endif

	/*
#ifndef DISABLE_SPIFFS
	// use user supplied values (defaults for 4mb flash in makefile)
	if (slot == 0) {
		otaUpdater->addItem(RBOOT_SPIFFS_0, SPIFFS_URL);
	} else {
		otaUpdater->addItem(RBOOT_SPIFFS_1, SPIFFS_URL);
	}
#endif
*/
	// request switch and reboot on success
	//otaUpdater->switchToRom(slot);
	// and/or set a callback (called on failure or success without switching requested)
	otaUpdater->setCallback(otaUpdateCb);

	// start update
	otaUpdater->start();
}


