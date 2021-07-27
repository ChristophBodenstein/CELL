################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
SPUO_SRCS += \
../spu_objs/spudnlrx.spuo \
../spu_objs/spuregler.spuo \
../spu_objs/spureglerLXYZ.spuo \
../spu_objs/spureglerNewThreads.spuo \
../spu_objs/spureglerWXYZ.spuo 

OBJS += \
./spu_objs/spudnlrx.o \
./spu_objs/spuregler.o \
./spu_objs/spureglerLXYZ.o \
./spu_objs/spureglerNewThreads.o \
./spu_objs/spureglerWXYZ.o 


# Each subdirectory must supply rules for building sources it contributes
spu_objs/%.o: ../spu_objs/%.spuo
	@echo 'Building file: $<'
	@echo 'Invoking: PPU GNU 32 bit Embed SPU'
	ppu-embedspu -m32 $(subst .,_,$(basename $(notdir $@))) $< $@
	@echo 'Finished building: $<'
	@echo ' '


