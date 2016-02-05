###########################################################################################
# MakeFile to create venmanager executable.
# Automatically compiles c amd cpp files upto one folder level in source folder
# 
############################################################################################

LIBS := -loadr -loadrsd -lxerces-c -lpthread -lcurl -lsqlite3 -lm
RM := rm -rf
INCLUDE_DEPS1 := ../oadr
INCLUDE_DEPS2 := ../oadrsd
LIBOADR := $(INCLUDE_DEPS1)/liboadr.so
LIBOADRSD := $(INCLUDE_DEPS2)/liboadrsd.so
MKDIR_P := mkdir -p
OBJ_DIR := Objfiles
GPP := g++
GCC := gcc

# Add inputs and outputs to the build variables 
srcdir := VENManager

OBJS_SRC := $(shell find $(srcdir) -name '*.cpp' -type f -printf "./$(OBJ_DIR)/%f ")
OBJS_SRC += $(shell find $(srcdir) -name '*.c' -type f -printf "./$(OBJ_DIR)/%f ")
OBJS_CPP := $(subst .cpp,.o,$(OBJS_SRC))
OBJS += $(subst .c,.o,$(OBJS_CPP))


# Rules for building subdirectory
$(OBJ_DIR)/%.o: $(srcdir)/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(GPP) -D__GXX_EXPERIMENTAL_CXX0X__ -I $(INCLUDE_DEPS1) -I $(INCLUDE_DEPS2) -O3 -Wall -c -fmessage-length=0 -std=c++0x -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Rules for building subdirectory
$(OBJ_DIR)/%.o: $(srcdir)/*/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(GPP) -D__GXX_EXPERIMENTAL_CXX0X__ -I $(INCLUDE_DEPS1) -I $(INCLUDE_DEPS2) -O3 -Wall -c -fmessage-length=0 -std=c++0x -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Rules for building subdirectory
$(OBJ_DIR)/%.o: $(srcdir)/*/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	$(GCC) -I $(INCLUDE_DEPS1) -I $(INCLUDE_DEPS2) -O3 -Wall -c -fmessage-length=0 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Rules for building .so file oadr.so and oadrsd.so if doesnot exist
%.so: 
ifeq ($(wildcard $(LIBOADR)),)
	@echo 'Invoking Makefile for liboadr.so'
	-cd $(INCLUDE_DEPS1) && $(MAKE) all
endif
ifeq ($(wildcard $(LIBOADRSD)),)
	@echo 'Invoking Makefile for liboadrsd.so'
	-cd $(INCLUDE_DEPS2) && $(MAKE) all
endif

# All Target
all: ${OBJ_DIR} venmanager 

${OBJ_DIR}: 
	${MKDIR_P} ${OBJ_DIR}

# Creating Executable file
venmanager: $(LIBOADR) $(LIBOADR) $(OBJS) 
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(GPP) -L $(INCLUDE_DEPS1) -L $(INCLUDE_DEPS2) -L/usr/local/include -L/usr/local/lib -o "venmanager" $(OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)  venmanager $(OBJ_DIR)
	-@echo ' '

.PHONY: all clean directories

