################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cr_startup_lpc8xx.c \
../src/main.c \
../src/sct_fsm.c 

OBJS += \
./src/cr_startup_lpc8xx.o \
./src/main.o \
./src/sct_fsm.o 

C_DEPS += \
./src/cr_startup_lpc8xx.d \
./src/main.d \
./src/sct_fsm.d 


# Each subdirectory must supply rules for building sources it contributes
src/cr_startup_lpc8xx.o: ../src/cr_startup_lpc8xx.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__MTB_DISABLE -D__MTB_BUFFER_SIZE=2048 -D__USE_CMSIS=CMSIS_CORE_LPC8xx -D__LPC8XX__ -I"/Users/cesare/Documents/Uni/MCUXpressoIDE_11.3.0/workspace/CMSIS_CORE_LPC8xx/inc" -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0 -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/cr_startup_lpc8xx.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__MTB_DISABLE -D__MTB_BUFFER_SIZE=2048 -D__USE_CMSIS=CMSIS_CORE_LPC8xx -D__LPC8XX__ -I"/Users/cesare/Documents/Uni/MCUXpressoIDE_11.3.0/workspace/CMSIS_CORE_LPC8xx/inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0 -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


