################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/adc_13xx.c \
../src/chip_13xx.c \
../src/clock_13xx.c \
../src/flash_13xx.c \
../src/gpio_13xx_1.c \
../src/gpio_13xx_2.c \
../src/gpiogroup_13xx.c \
../src/i2c_13xx.c \
../src/i2cm_13xx.c \
../src/iocon_13xx.c \
../src/pinint_13xx.c \
../src/pmu_13xx.c \
../src/ring_buffer.c \
../src/ritimer_13xx.c \
../src/ssp_13xx.c \
../src/stopwatch_13xx.c \
../src/sysctl_13xx.c \
../src/sysinit_13xx.c \
../src/timer_13xx.c \
../src/uart_13xx.c \
../src/wwdt_13xx.c 

OBJS += \
./src/adc_13xx.o \
./src/chip_13xx.o \
./src/clock_13xx.o \
./src/flash_13xx.o \
./src/gpio_13xx_1.o \
./src/gpio_13xx_2.o \
./src/gpiogroup_13xx.o \
./src/i2c_13xx.o \
./src/i2cm_13xx.o \
./src/iocon_13xx.o \
./src/pinint_13xx.o \
./src/pmu_13xx.o \
./src/ring_buffer.o \
./src/ritimer_13xx.o \
./src/ssp_13xx.o \
./src/stopwatch_13xx.o \
./src/sysctl_13xx.o \
./src/sysinit_13xx.o \
./src/timer_13xx.o \
./src/uart_13xx.o \
./src/wwdt_13xx.o 

C_DEPS += \
./src/adc_13xx.d \
./src/chip_13xx.d \
./src/clock_13xx.d \
./src/flash_13xx.d \
./src/gpio_13xx_1.d \
./src/gpio_13xx_2.d \
./src/gpiogroup_13xx.d \
./src/i2c_13xx.d \
./src/i2cm_13xx.d \
./src/iocon_13xx.d \
./src/pinint_13xx.d \
./src/pmu_13xx.d \
./src/ring_buffer.d \
./src/ritimer_13xx.d \
./src/ssp_13xx.d \
./src/stopwatch_13xx.d \
./src/sysctl_13xx.d \
./src/sysinit_13xx.d \
./src/timer_13xx.d \
./src/uart_13xx.d \
./src/wwdt_13xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DNDEBUG -D__CODE_RED -DCORE_M3 -I"C:\nxp\workspace\lpc1347\lpc_chip_13xx\inc" -Os -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


