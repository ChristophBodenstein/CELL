################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../functionsvectorLXYZ.c \
../spureglerLXYZ.c 

OBJS += \
./functionsvectorLXYZ.o \
./spureglerLXYZ.o 

C_DEPS += \
./functionsvectorLXYZ.d \
./spureglerLXYZ.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: SPU GNU C Compiler with Debug Options'
	spu-gcc -I"/home/bode/workspace/regler" -O0 -g3 -Wall -c -fmessage-length=0 -Winline -Wextra -fno-inline -mtune=cell -mfloat=fast -mdouble=fast -Wno-main -march=cell -mea32 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


