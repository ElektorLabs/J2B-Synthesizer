#ifndef __EEPROM_H__
#define __EEPROM_H__


// TODO: replace this structure by external EEPROM chip driver.
typedef struct
{
	void (*write)(uint32_t address, uint8_t value);
	uint8_t (*read)(uint32_t address);
}
eeprom_t;

extern eeprom_t EEPROM;


#endif // __EEPROM_H__
