#include "atmegatron.h"
#include "libeeprom.h"

// The top 64 bytes of the EEPROM memory are reserved and cannot be written to.
#define EEPROM_ADDRESS_OFFSET  (0x40)


void eeprom_write(uint32_t address, uint8_t value);
uint8_t eeprom_read(uint32_t address);


eeprom_t EEPROM =
{
	eeprom_write,
	eeprom_read,
};


/*void eeprom_execute(EELIB_Command pCommand, EELIB_Result pOutput)
{
	// Use libeeprom instead of the IAP driver.
	// Now you don't have to disable interrupts anymore.
	//__disable_irq();
	EELIB_entry(pCommand,pOutput);
	//__enable_irq();
}*/


void eeprom_write(uint32_t address, uint8_t value)
{
	EEPROM_WRITE_COMMAND_T command;
	EEPROM_WRITE_OUTPUT_T result;

	command.cmd = EELIB_IAP_COMMAND_EEPROM_WRITE;
	command.eepromAddr = address + EEPROM_ADDRESS_OFFSET;
	command.ramAddr = (uint32_t) &value;
	command.byteNum = sizeof(value);
	command.cclk = Chip_Clock_GetSystemClockRate() / 1000;
	EELIB_entry((uint32_t*)&command,(uint32_t*)&result);
	if (result.status!=EELIB_IAP_STATUS_CMD_SUCCESS)
	{
		// Do something on error?
	}
}


uint8_t eeprom_read(uint32_t address)
{
	uint8_t buffer;
	EEPROM_READ_COMMAND_T command;
	EEPROM_READ_OUTPUT_T result;

	command.cmd = EELIB_IAP_COMMAND_EEPROM_READ;
	command.eepromAddr = address + EEPROM_ADDRESS_OFFSET;
	command.ramAddr = (uint32_t) &buffer;
	command.byteNum = sizeof(buffer);
	command.cclk = Chip_Clock_GetSystemClockRate() / 1000;
	EELIB_entry((uint32_t*)&command,(uint32_t*)&result);
	if (result.status!=EELIB_IAP_STATUS_CMD_SUCCESS)
	{
		buffer = 0;
	}
	return buffer;
}
