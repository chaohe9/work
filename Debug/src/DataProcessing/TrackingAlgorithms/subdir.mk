################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DataProcessing/TrackingAlgorithms/KalmanFilter.cpp 

OBJS += \
./src/DataProcessing/TrackingAlgorithms/KalmanFilter.o 

CPP_DEPS += \
./src/DataProcessing/TrackingAlgorithms/KalmanFilter.d 


# Each subdirectory must supply rules for building sources it contributes
src/DataProcessing/TrackingAlgorithms/%.o: ../src/DataProcessing/TrackingAlgorithms/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/oubaituo/IbeoSDK5.0.4/src -I/home/oubaituo/workspace/oubaituoSDK/src -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


