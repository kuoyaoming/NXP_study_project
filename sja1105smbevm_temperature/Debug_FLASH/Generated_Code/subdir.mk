################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Generated_Code/Cpu.c \
../Generated_Code/can.c \
../Generated_Code/clockMan.c \
../Generated_Code/dmaController1.c \
../Generated_Code/enet_dbg.c \
../Generated_Code/enet_sw.c \
../Generated_Code/phy_cfg.c \
../Generated_Code/pin_mux.c \
../Generated_Code/spi_can.c \
../Generated_Code/spi_sw.c \
../Generated_Code/uart.c 

OBJS += \
./Generated_Code/Cpu.o \
./Generated_Code/can.o \
./Generated_Code/clockMan.o \
./Generated_Code/dmaController1.o \
./Generated_Code/enet_dbg.o \
./Generated_Code/enet_sw.o \
./Generated_Code/phy_cfg.o \
./Generated_Code/pin_mux.o \
./Generated_Code/spi_can.o \
./Generated_Code/spi_sw.o \
./Generated_Code/uart.o 

C_DEPS += \
./Generated_Code/Cpu.d \
./Generated_Code/can.d \
./Generated_Code/clockMan.d \
./Generated_Code/dmaController1.d \
./Generated_Code/enet_dbg.d \
./Generated_Code/enet_sw.d \
./Generated_Code/phy_cfg.d \
./Generated_Code/pin_mux.d \
./Generated_Code/spi_can.d \
./Generated_Code/spi_sw.d \
./Generated_Code/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Generated_Code/%.o: ../Generated_Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	powerpc-eabivle-gcc "@Generated_Code/Cpu.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


