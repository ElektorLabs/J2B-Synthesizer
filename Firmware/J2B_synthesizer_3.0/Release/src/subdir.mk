################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/amplitude.c \
../src/arpeggiator.c \
../src/bitcrush.c \
../src/cr_startup_lpc13xx.c \
../src/cycle.c \
../src/delay.c \
../src/distortion.c \
../src/eeprom.c \
../src/envelope.c \
../src/envelope2.c \
../src/filter.c \
../src/flange.c \
../src/hardware.c \
../src/keyboard.c \
../src/lcd.c \
../src/lfo.c \
../src/main.c \
../src/master.c \
../src/memory.c \
../src/midi.c \
../src/pitch.c \
../src/pwm.c \
../src/pwm_timer.c \
../src/sample_rate.c \
../src/sequencer.c \
../src/serial.c \
../src/sysinit.c \
../src/systick.c \
../src/testmode.c \
../src/user_interface.c \
../src/util.c \
../src/waveforms.c \
../src/waveforms_delayertron.c \
../src/wavestore.c 

OBJS += \
./src/amplitude.o \
./src/arpeggiator.o \
./src/bitcrush.o \
./src/cr_startup_lpc13xx.o \
./src/cycle.o \
./src/delay.o \
./src/distortion.o \
./src/eeprom.o \
./src/envelope.o \
./src/envelope2.o \
./src/filter.o \
./src/flange.o \
./src/hardware.o \
./src/keyboard.o \
./src/lcd.o \
./src/lfo.o \
./src/main.o \
./src/master.o \
./src/memory.o \
./src/midi.o \
./src/pitch.o \
./src/pwm.o \
./src/pwm_timer.o \
./src/sample_rate.o \
./src/sequencer.o \
./src/serial.o \
./src/sysinit.o \
./src/systick.o \
./src/testmode.o \
./src/user_interface.o \
./src/util.o \
./src/waveforms.o \
./src/waveforms_delayertron.o \
./src/wavestore.o 

C_DEPS += \
./src/amplitude.d \
./src/arpeggiator.d \
./src/bitcrush.d \
./src/cr_startup_lpc13xx.d \
./src/cycle.d \
./src/delay.d \
./src/distortion.d \
./src/eeprom.d \
./src/envelope.d \
./src/envelope2.d \
./src/filter.d \
./src/flange.d \
./src/hardware.d \
./src/keyboard.d \
./src/lcd.d \
./src/lfo.d \
./src/main.d \
./src/master.d \
./src/memory.d \
./src/midi.d \
./src/pitch.d \
./src/pwm.d \
./src/pwm_timer.d \
./src/sample_rate.d \
./src/sequencer.d \
./src/serial.d \
./src/sysinit.d \
./src/systick.d \
./src/testmode.d \
./src/user_interface.d \
./src/util.d \
./src/waveforms.d \
./src/waveforms_delayertron.d \
./src/wavestore.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DNDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M3 -I"C:\nxp\workspace\lpc1347\lpc_chip_13xx\inc" -I"C:\nxp\workspace\lpc1347\lpc_board_elektor_140182\inc" -I"C:\nxp\workspace\lpc1347\J2B_synthesizer_3.0\inc" -I"C:\nxp\workspace\lpc1347\J2B_synthesizer_3.0\libeeprom-v1_0" -Os -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


