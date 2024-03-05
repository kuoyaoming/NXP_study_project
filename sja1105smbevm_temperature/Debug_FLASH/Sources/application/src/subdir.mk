################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/application/src/can_app.c \
../Sources/application/src/enet_app.c \
../Sources/application/src/enet_callbacks.c \
../Sources/application/src/phy_callbacks.c 

OBJS += \
./Sources/application/src/can_app.o \
./Sources/application/src/enet_app.o \
./Sources/application/src/enet_callbacks.o \
./Sources/application/src/phy_callbacks.o 

C_DEPS += \
./Sources/application/src/can_app.d \
./Sources/application/src/enet_app.d \
./Sources/application/src/enet_callbacks.d \
./Sources/application/src/phy_callbacks.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/application/src/%.o: ../Sources/application/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	powerpc-eabivle-gcc "@Sources/application/src/can_app.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


