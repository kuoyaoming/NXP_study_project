################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Sources/sja1105smbevm/src/subdir.mk
-include Sources/sja1105PQRS_driver/src/low_level_driver/subdir.mk
-include Sources/sja1105PQRS_driver/src/subdir.mk
-include Sources/sja1105PQRS_driver/configtools/examples_SJA1105x/subdir.mk
-include Sources/sja1105PQRS_driver/configtools/subdir.mk
-include Sources/sja1105PQRS_driver/subdir.mk
-include Sources/application/src/subdir.mk
-include Sources/subdir.mk
-include SDK/rtos/osif/subdir.mk
-include SDK/rtos/FreeRTOS_PA/Source/portable/MemMang/subdir.mk
-include SDK/rtos/FreeRTOS_PA/Source/portable/GCC/PowerPC/subdir.mk
-include SDK/rtos/FreeRTOS_PA/Source/subdir.mk
-include SDK/platform/pal/src/uart/subdir.mk
-include SDK/platform/drivers/src/pins/siul2/subdir.mk
-include SDK/platform/drivers/src/phy/subdir.mk
-include SDK/platform/drivers/src/linflexd/subdir.mk
-include SDK/platform/drivers/src/interrupt/subdir.mk
-include SDK/platform/drivers/src/flexcan/subdir.mk
-include SDK/platform/drivers/src/enet/subdir.mk
-include SDK/platform/drivers/src/edma/subdir.mk
-include SDK/platform/drivers/src/dspi/subdir.mk
-include SDK/platform/drivers/src/clock/MPC57xx/subdir.mk
-include SDK/platform/devices/MPC5748G/startup/subdir.mk
-include SDK/platform/devices/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include Project_Settings/Linker_Files/subdir.mk
-include Generated_Code/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
sja1105smbevm_tc10example.srec \

SECONDARY_SIZE += \
sja1105smbevm_tc10example.siz \


# All Target
all: sja1105smbevm_tc10example.elf secondary-outputs

# Tool invocations
sja1105smbevm_tc10example.elf: $(OBJS) C:/Users/nxf99725/workspaceS32DS.Power.2.1/sja1105smbevm_tc10example/Project_Settings/Linker_Files/linker_flash.ld $(OSPT) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Standard S32DS C Linker'
	powerpc-eabivle-gcc -o "sja1105smbevm_tc10example.elf" "@sja1105smbevm_tc10example.args"  $(USER_OBJS) $(LIBS) $(OSPT)
	@echo 'Finished building target: $@'
	@echo ' '

sja1105smbevm_tc10example.srec: sja1105smbevm_tc10example.elf
	@echo 'Invoking: Standard S32DS Create Flash Image'
	powerpc-eabivle-objcopy -O srec sja1105smbevm_tc10example.elf  "sja1105smbevm_tc10example.srec"
	@echo 'Finished building: $@'
	@echo ' '

sja1105smbevm_tc10example.siz: sja1105smbevm_tc10example.elf
	@echo 'Invoking: Standard S32DS Print Size'
	powerpc-eabivle-size --format=berkeley sja1105smbevm_tc10example.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
