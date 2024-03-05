################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_auxiliaryConfigurationUnit.c \
../Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_clockGenerationUnit.c \
../Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_resetGenerationUnit.c \
../Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_sgmii.c \
../Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_spi.c \
../Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_switchCore.c 

OBJS += \
./Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_auxiliaryConfigurationUnit.o \
./Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_clockGenerationUnit.o \
./Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_resetGenerationUnit.o \
./Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_sgmii.o \
./Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_spi.o \
./Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_switchCore.o 

C_DEPS += \
./Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_auxiliaryConfigurationUnit.d \
./Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_clockGenerationUnit.d \
./Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_resetGenerationUnit.d \
./Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_sgmii.d \
./Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_spi.d \
./Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_switchCore.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/sja1105PQRS_driver/src/low_level_driver/%.o: ../Sources/sja1105PQRS_driver/src/low_level_driver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	powerpc-eabivle-gcc "@Sources/sja1105PQRS_driver/src/low_level_driver/NXP_SJA1105P_auxiliaryConfigurationUnit.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


