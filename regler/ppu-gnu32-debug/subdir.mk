################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../dnlrx.c \
../dnlx-pid.c \
../functionsnormal.c \
../functionsvector.c \
../local_spuregler.c \
../regler.c \
../timing.c 

OBJS += \
./dnlrx.o \
./dnlx-pid.o \
./functionsnormal.o \
./functionsvector.o \
./local_spuregler.o \
./regler.o \
./timing.o 

C_DEPS += \
./dnlrx.d \
./dnlx-pid.d \
./functionsnormal.d \
./functionsvector.d \
./local_spuregler.d \
./regler.d \
./timing.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: PPU GNU 32 bit C Compiler with Debug Options'
	ppu-gcc -I/opt/cell/sdk/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -Winline -Wextra -fno-inline -m32 -mabi=altivec -maltivec -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


