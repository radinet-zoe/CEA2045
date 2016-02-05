################################################################################
# MakeFile to create CyUSBSerialTestUtility 
# Requirement - libcyusbserial.so
################################################################################

RM := rm -rf
INCLUDE_DEPS := common/header
srcdir := linux/testUtility
LIBS := -lcyusbserial

SRC_FILE := ${srcdir}/Test_Utility.c

GCC := gcc

all: CyUSBSerialTestUtility

CyUSBSerialTestUtility:
	@echo 'Building target: $@'
	@echo 'Invoking: C Library'
	$(GCC) -g -o "$@" $(SRC_FILE) $(LIBS) -w
	@echo 'Finished building target: $@'
	@echo ' '

clean:
	-$(RM) CyUSBSerialTestUtility
	-@echo ' '

help:
	@echo	'make		would compile and create the CyUSBSerialTestUtility'
	@echo	'make clean	would remove the CyUSBSerialTestUtility'
