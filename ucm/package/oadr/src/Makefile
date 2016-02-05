################################################################################
# MakeFile to create liboadr.so.
# Requirement - oadrsd folder, oadr folder, xsd folder
# Folder Structure
# ----
#     |--- oadr---
#		  |---oadr---
# 	             	     |---folder structure - cpp and h Files
#		  |---liboadr.so
#		  |---Makefile
#     |--- oadrsd
#		  |---oadrsd---
#			       |--- folder structure - cxx and hxx files
#		  |---xsd---
#			    |--- cxx
#		  |---liboadrsd.so
#		  |---Makefile
################################################################################

RM := rm -rf
INCLUDE_DEPS := ../oadrsd
MKDIR_P := mkdir -p
OBJ_DIR := Objfiles
GPP := g++

# Add inputs and outputs to the build variables 
CPP_SRCS := \
oadr/helper/RandomHex.cpp \
oadr/request/opt/CancelOptSchedule.cpp \
oadr/request/opt/CreateOpt.cpp \
oadr/request/opt/CreateOptEvent.cpp \
oadr/request/opt/CreateOptSchedule.cpp \
oadr/request/opt/OptReasonValue.cpp \
oadr/request/opt/OptSchedule.cpp \
oadr/request/report/DataQualityTypeValue.cpp \
oadr/request/report/ReadingTypeEnumeratedTypeValue.cpp \
oadr/request/report/RegisterReport.cpp \
oadr/request/report/RegisteredReport.cpp \
oadr/request/report/ReportEnumeratedTypeValue.cpp \
oadr/request/report/ReportName.cpp \
oadr/request/report/ReportWrapper.cpp \
oadr/request/report/ReportWrapperList.cpp \
oadr/request/report/UpdateReport.cpp \
oadr/request/CancelPartyRegistration.cpp \
oadr/request/CanceledReport.cpp \
oadr/request/CreatePartyRegistration.cpp \
oadr/request/CreatedEvent.cpp \
oadr/request/CreatedReport.cpp \
oadr/request/DurationModifier.cpp \
oadr/request/EventResponses.cpp \
oadr/request/Oadr2bHelper.cpp \
oadr/request/Oadr2bRequest.cpp \
oadr/request/OadrException.cpp \
oadr/request/Poll.cpp \
oadr/request/QueryRegistration.cpp \
oadr/request/RequestEvent.cpp \
oadr/ven/http/CurlBuffer.cpp \
oadr/ven/http/CurlException.cpp \
oadr/ven/http/CurlReceiveBuffer.cpp \
oadr/ven/http/CurlSendBuffer.cpp \
oadr/ven/http/HttpCurl.cpp \
oadr/ven/http/IHttp.cpp \
oadr/ven/VEN2b.cpp 

OBJS := \
./$(OBJ_DIR)/RandomHex.o \
./$(OBJ_DIR)/CancelOptSchedule.o \
./$(OBJ_DIR)/CreateOpt.o \
./$(OBJ_DIR)/CreateOptEvent.o \
./$(OBJ_DIR)/CreateOptSchedule.o \
./$(OBJ_DIR)/OptReasonValue.o \
./$(OBJ_DIR)/OptSchedule.o \
./$(OBJ_DIR)/DataQualityTypeValue.o \
./$(OBJ_DIR)/ReadingTypeEnumeratedTypeValue.o \
./$(OBJ_DIR)/RegisterReport.o \
./$(OBJ_DIR)/RegisteredReport.o \
./$(OBJ_DIR)/ReportEnumeratedTypeValue.o \
./$(OBJ_DIR)/ReportName.o \
./$(OBJ_DIR)/ReportWrapper.o \
./$(OBJ_DIR)/ReportWrapperList.o \
./$(OBJ_DIR)/UpdateReport.o \
./$(OBJ_DIR)/CancelPartyRegistration.o \
./$(OBJ_DIR)/CanceledReport.o \
./$(OBJ_DIR)/CreatePartyRegistration.o \
./$(OBJ_DIR)/CreatedEvent.o \
./$(OBJ_DIR)/CreatedReport.o \
./$(OBJ_DIR)/DurationModifier.o \
./$(OBJ_DIR)/EventResponses.o \
./$(OBJ_DIR)/Oadr2bHelper.o \
./$(OBJ_DIR)/Oadr2bRequest.o \
./$(OBJ_DIR)/OadrException.o \
./$(OBJ_DIR)/Poll.o \
./$(OBJ_DIR)/QueryRegistration.o \
./$(OBJ_DIR)/RequestEvent.o \
./$(OBJ_DIR)/CurlBuffer.o \
./$(OBJ_DIR)/CurlException.o \
./$(OBJ_DIR)/CurlReceiveBuffer.o \
./$(OBJ_DIR)/CurlSendBuffer.o \
./$(OBJ_DIR)/HttpCurl.o \
./$(OBJ_DIR)/IHttp.o \
./$(OBJ_DIR)/VEN2b.o 

CPP_DEPS := \
./$(OBJ_DIR)/RandomHex.d \
./$(OBJ_DIR)/CancelOptSchedule.d \
./$(OBJ_DIR)/CreateOpt.d \
./$(OBJ_DIR)/CreateOptEvent.d \
./$(OBJ_DIR)/CreateOptSchedule.d \
./$(OBJ_DIR)/OptReasonValue.d \
./$(OBJ_DIR)/OptSchedule.d \
./$(OBJ_DIR)/DataQualityTypeValue.d \
./$(OBJ_DIR)/ReadingTypeEnumeratedTypeValue.d \
./$(OBJ_DIR)/RegisterReport.d \
./$(OBJ_DIR)/RegisteredReport.d \
./$(OBJ_DIR)/ReportEnumeratedTypeValue.d \
./$(OBJ_DIR)/ReportName.d \
./$(OBJ_DIR)/ReportWrapper.d \
./$(OBJ_DIR)/ReportWrapperList.d \
./$(OBJ_DIR)/UpdateReport.d \
./$(OBJ_DIR)/CancelPartyRegistration.d \
./$(OBJ_DIR)/CanceledReport.d \
./$(OBJ_DIR)/CreatePartyRegistration.d \
./$(OBJ_DIR)/CreatedEvent.d \
./$(OBJ_DIR)/CreatedReport.d \
./$(OBJ_DIR)/DurationModifier.d \
./$(OBJ_DIR)/EventResponses.d \
./$(OBJ_DIR)/Oadr2bHelper.d \
./$(OBJ_DIR)/Oadr2bRequest.d \
./$(OBJ_DIR)/OadrException.d \
./$(OBJ_DIR)/Poll.d \
./$(OBJ_DIR)/QueryRegistration.d \
./$(OBJ_DIR)/RequestEvent.d \
./$(OBJ_DIR)/CurlBuffer.d \
./$(OBJ_DIR)/CurlException.d \
./$(OBJ_DIR)/CurlReceiveBuffer.d \
./$(OBJ_DIR)/CurlSendBuffer.d \
./$(OBJ_DIR)/HttpCurl.d \
./$(OBJ_DIR)/IHttp.d \
./$(OBJ_DIR)/VEN2b.d

# Rules for building subdirectory
$(OBJ_DIR)/%.o: oadr/helper/%.cpp 
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(GPP) -I $(INCLUDE_DEPS)  -L/usr/include -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Rules for building subdirectory
$(OBJ_DIR)/%.o: oadr/request/opt/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(GPP) -I $(INCLUDE_DEPS) -L/usr/include  -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Rules for building subdirectory
$(OBJ_DIR)/%.o: oadr/request/report/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(GPP) -I $(INCLUDE_DEPS) -L/usr/include  -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Rules for building subdirectory
$(OBJ_DIR)/%.o: oadr/request/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(GPP) -I $(INCLUDE_DEPS) -L/usr/include  -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Rules for building subdirectory
$(OBJ_DIR)/%.o: oadr/ven/http/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(GPP) -I $(INCLUDE_DEPS)  -L/usr/include -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Rules for building subdirectory
$(OBJ_DIR)/%.o: oadr/ven/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(GPP) -I $(INCLUDE_DEPS) -L/usr/include  -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


# All Target
all: directories liboadr.so

directories: 
	${MKDIR_P} ${OBJ_DIR}

# Creating .so filr
liboadr.so: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(GPP) -shared -o "liboadr.so" $(OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(CPP_DEPS) liboadr.so $(OBJ_DIR)
	-@echo ' '

.PHONY: all clean directories

