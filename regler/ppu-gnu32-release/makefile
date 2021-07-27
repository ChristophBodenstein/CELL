################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Custom includes added by CellDT makefile generator
-include import_spu.mk

# Macros added by the CDT's makefile generator

-include ../makefile.init

RM := rm -fr

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include spu_objs/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: regler

dependents:
	-cd /home/bode/workspace/spureglerLXYZ/spu-gnu-release && $(MAKE) clean all
	-cd /home/bode/workspace/spureglerNewThreads/spu-gnu-release && $(MAKE) clean all
	-cd /home/bode/workspace/spuregler/spu-gnu-release && $(MAKE) clean all
	-cd /home/bode/workspace/spudnlrx/spu-gnu-release && $(MAKE) clean all
	-cd /home/bode/workspace/spureglerWXYZ/spu-gnu-release && $(MAKE) clean all

# Tool invocations
regler: $(OBJS) $(USER_OBJS) /home/bode/workspace/spureglerLXYZ/spu-gnu-release/spureglerLXYZ /home/bode/workspace/spureglerNewThreads/spu-gnu-release/spureglerNewThreads /home/bode/workspace/spuregler/spu-gnu-release/spuregler /home/bode/workspace/spudnlrx/spu-gnu-release/spudnlrx /home/bode/workspace/spureglerWXYZ/spu-gnu-release/spureglerWXYZ
	@echo 'Building target: $@'
	@echo 'Invoking: PPU GNU 32 bit C Linker'
	ppu-gcc -L/usr/lib -m32 -o"regler" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) regler
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/bode/workspace/spureglerLXYZ/spu-gnu-release/spureglerLXYZ:
/home/bode/workspace/spureglerNewThreads/spu-gnu-release/spureglerNewThreads:
/home/bode/workspace/spuregler/spu-gnu-release/spuregler:
/home/bode/workspace/spudnlrx/spu-gnu-release/spudnlrx:
/home/bode/workspace/spureglerWXYZ/spu-gnu-release/spureglerWXYZ:

-include ../makefile.targets
