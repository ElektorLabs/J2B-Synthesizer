################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: J2B_synthesizer.axf

# Tool invocations
J2B_synthesizer.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\nxp\workspace\lpc1347\lpc_chip_13xx\Debug" -L"C:\nxp\workspace\lpc1347\lpc_board_elektor_140182\Debug" -Xlinker -Map="J2B_synthesizer.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "J2B_synthesizer_Debug.ld" -o "J2B_synthesizer.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) J2B_synthesizer.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "J2B_synthesizer.axf"; arm-none-eabi-objcopy -O binary "J2B_synthesizer.axf" "J2B_synthesizer.bin" ; checksum -p LPC1347 -d "J2B_synthesizer.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
