################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/cr_startup_lpc13xx.c \
../example/src/freertos_blinky.c \
../example/src/sysinit.c 

OBJS += \
./example/src/cr_startup_lpc13xx.o \
./example/src/freertos_blinky.o \
./example/src/sysinit.o 

C_DEPS += \
./example/src/cr_startup_lpc13xx.d \
./example/src/freertos_blinky.d \
./example/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M3 -I"/tmp/lpc4337_test/lpc_chip_13xx/inc" -I"/tmp/lpc4337_test/lpc_board_nxp_lpcxpresso_1343/inc" -I"/tmp/lpc4337_test/freertos_blinky/example/inc" -I"/tmp/lpc4337_test/freertos_blinky/freertos/inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


