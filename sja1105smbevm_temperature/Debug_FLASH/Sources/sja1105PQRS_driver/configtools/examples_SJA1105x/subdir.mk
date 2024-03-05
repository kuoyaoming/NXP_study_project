################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/sja1105PQRS_driver/configtools/examples_SJA1105x/hex_to_c.c 

OBJS += \
./Sources/sja1105PQRS_driver/configtools/examples_SJA1105x/hex_to_c.o 

C_DEPS += \
./Sources/sja1105PQRS_driver/configtools/examples_SJA1105x/hex_to_c.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/sja1105PQRS_driver/configtools/examples_SJA1105x/%.o: ../Sources/sja1105PQRS_driver/configtools/examples_SJA1105x/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	powerpc-eabivle-gcc "@Sources/sja1105PQRS_driver/configtools/examples_SJA1105x/hex_to_c.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


