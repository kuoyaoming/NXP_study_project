################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SDK/platform/drivers/src/phy/phy.c \
../SDK/platform/drivers/src/phy/phy_generic.c \
../SDK/platform/drivers/src/phy/phy_shared.c \
../SDK/platform/drivers/src/phy/phy_tja110x.c 

OBJS += \
./SDK/platform/drivers/src/phy/phy.o \
./SDK/platform/drivers/src/phy/phy_generic.o \
./SDK/platform/drivers/src/phy/phy_shared.o \
./SDK/platform/drivers/src/phy/phy_tja110x.o 

C_DEPS += \
./SDK/platform/drivers/src/phy/phy.d \
./SDK/platform/drivers/src/phy/phy_generic.d \
./SDK/platform/drivers/src/phy/phy_shared.d \
./SDK/platform/drivers/src/phy/phy_tja110x.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/drivers/src/phy/%.o: ../SDK/platform/drivers/src/phy/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	powerpc-eabivle-gcc "@SDK/platform/drivers/src/phy/phy.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


