################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/sja1105PQRS_driver/src/NXP_SJA1105P_addressResolutionTable.c \
../Sources/sja1105PQRS_driver/src/NXP_SJA1105P_cbs.c \
../Sources/sja1105PQRS_driver/src/NXP_SJA1105P_config.c \
../Sources/sja1105PQRS_driver/src/NXP_SJA1105P_diagnostics.c \
../Sources/sja1105PQRS_driver/src/NXP_SJA1105P_ethIf.c \
../Sources/sja1105PQRS_driver/src/NXP_SJA1105P_mgmtRoutes.c \
../Sources/sja1105PQRS_driver/src/NXP_SJA1105P_portConfig.c \
../Sources/sja1105PQRS_driver/src/NXP_SJA1105P_ptp.c \
../Sources/sja1105PQRS_driver/src/NXP_SJA1105P_utils.c \
../Sources/sja1105PQRS_driver/src/NXP_SJA1105P_vlan.c 

OBJS += \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_addressResolutionTable.o \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_cbs.o \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_config.o \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_diagnostics.o \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_ethIf.o \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_mgmtRoutes.o \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_portConfig.o \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_ptp.o \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_utils.o \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_vlan.o 

C_DEPS += \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_addressResolutionTable.d \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_cbs.d \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_config.d \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_diagnostics.d \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_ethIf.d \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_mgmtRoutes.d \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_portConfig.d \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_ptp.d \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_utils.d \
./Sources/sja1105PQRS_driver/src/NXP_SJA1105P_vlan.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/sja1105PQRS_driver/src/%.o: ../Sources/sja1105PQRS_driver/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	powerpc-eabivle-gcc "@Sources/sja1105PQRS_driver/src/NXP_SJA1105P_addressResolutionTable.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


