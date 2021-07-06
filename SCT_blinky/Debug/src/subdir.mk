################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cr_startup_lpc8xx.c \
../src/main.c \
../src/sct_fsm.c \
../src/swm.c 

OBJS += \
./src/cr_startup_lpc8xx.o \
./src/main.o \
./src/sct_fsm.o \
./src/swm.o 

C_DEPS += \
./src/cr_startup_lpc8xx.d \
./src/main.d \
./src/sct_fsm.d \
./src/swm.d 


# Each subdirectory must supply rules for building sources it contributes
src/cr_startup_lpc8xx.o: ../src/cr_startup_lpc8xx.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSIS_CORE_LPC8xx -D__LPC8XX__ -I"C:\SCT Cookbook code examples\LPCXpresso\SCT_blinky_match\CMSIS_CORE_LPC8xx\inc" -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/cr_startup_lpc8xx.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSIS_CORE_LPC8xx -D__LPC8XX__ -I"C:\SCT Cookbook code examples\LPCXpresso\SCT_blinky_match\CMSIS_CORE_LPC8xx\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


