################################################################################
# MakeFile to create libcyusbserial.so
# Requirement - libusb-1.0.9
################################################################################

RM := rm -rf
MKDIR_P := mkdir -p
OBJ_DIR := Objfiles
INCLUDE_DEPS := common/header
srcdir := linux/library
LIBS := -lusb-1.0

OBJS_SRC := $(shell find $(srcdir) -name '*.c' -type f -printf "./$(OBJ_DIR)/%f ")
OBJS += $(subst .c,.o,$(OBJS_SRC))

GCC := gcc

all: ${OBJ_DIR} libcyusbserial.so


${OBJ_DIR}:
	${MKDIR_P} ${OBJ_DIR}

# Rules for building subdirectory
$(OBJ_DIR)/%.o: ${srcdir}/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: C Compiler'
	$(GCC) -fPIC -g -Wall -o "$@" -c "$<" -I $(INCLUDE_DEPS)
	@echo 'Finished building: $<'
	@echo ' '

libcyusbserial.so:$(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(GCC) -shared -o "$@" $(OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

clean:
	-$(RM) $(OBJS) libcyusbserial.so $(OBJ_DIR)
	-@echo ' '

help:
	@echo	'make		would compile and create the library and create a link'
	@echo	'make clean	would remove the library and the soft link to the library (soname)'
