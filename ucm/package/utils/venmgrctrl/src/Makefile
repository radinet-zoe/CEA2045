################################################################################
# MakeFile to create CyUSBSerialTestUtility 
# Requirement - libcyusbserial.so
################################################################################

RM := rm -rf
#INCLUDE_DEPS := common/header
srcdir := ./
#LIBS := -lcyusbserial

SRC_FILE := ${srcdir}/venmgrcontrol.c

GCC := gcc

all: venmgrctrl

venmgrctrl:
	@echo 'Building target: $@'
	@echo 'Invoking: C Library'
	$(GCC) -g -o "$@" $(SRC_FILE) $(LIBS) -w
	@echo 'Finished building target: $@'
	@echo ' '

clean:
	-$(RM) venmgrctrl
	-@echo ' '

help:
	@echo	'make		would compile and create the venmgrctrl'
	@echo	'make clean	would remove the venmgrctrl'
