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
all: SCT_blinky_irq.axf

# Tool invocations
SCT_blinky_irq.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -L"/Users/cesare/Documents/Uni/MCUXpressoIDE_11.3.0/workspace/CMSIS_CORE_LPC8xx/Debug" -Xlinker -Map="SCT_blinky_irq.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "SCT_blinky_irq_Debug.ld" -o "SCT_blinky_irq.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) SCT_blinky_irq.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "SCT_blinky_irq.axf"; # arm-none-eabi-objcopy -O binary "SCT_blinky_irq.axf" "SCT_blinky_irq.bin" ; checksum -p LPC812 -d "SCT_blinky_irq.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
