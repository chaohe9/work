################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DataProcessing/Egomotion/Egomotion.cpp 

OBJS += \
./src/DataProcessing/Egomotion/Egomotion.o 

CPP_DEPS += \
./src/DataProcessing/Egomotion/Egomotion.d 


# Each subdirectory must supply rules for building sources it contributes
src/DataProcessing/Egomotion/%.o: ../src/DataProcessing/Egomotion/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/oubaituo/IbeoSDK5.0.4/src -I/home/oubaituo/workspace/oubaituoSDK/src -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


