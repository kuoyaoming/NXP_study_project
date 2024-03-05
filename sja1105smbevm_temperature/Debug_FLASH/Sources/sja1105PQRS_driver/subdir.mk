################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/sja1105PQRS_driver/NXP_SJA1105P_configStream.c 

OBJS += \
./Sources/sja1105PQRS_driver/NXP_SJA1105P_configStream.o 

C_DEPS += \
./Sources/sja1105PQRS_driver/NXP_SJA1105P_configStream.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/sja1105PQRS_driver/%.o: ../Sources/sja1105PQRS_driver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	powerpc-eabivle-gcc "@Sources/sja1105PQRS_driver/NXP_SJA1105P_configStream.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


