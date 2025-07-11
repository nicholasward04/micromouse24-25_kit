################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/main.c \
../Core/Src/mm_commands.c \
../Core/Src/mm_encoders.c \
../Core/Src/mm_floodfill.c \
../Core/Src/mm_motors.c \
../Core/Src/mm_profiles.c \
../Core/Src/mm_supplemental.c \
../Core/Src/mm_systick.c \
../Core/Src/mm_vision.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c 

OBJS += \
./Core/Src/main.o \
./Core/Src/mm_commands.o \
./Core/Src/mm_encoders.o \
./Core/Src/mm_floodfill.o \
./Core/Src/mm_motors.o \
./Core/Src/mm_profiles.o \
./Core/Src/mm_supplemental.o \
./Core/Src/mm_systick.o \
./Core/Src/mm_vision.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o 

C_DEPS += \
./Core/Src/main.d \
./Core/Src/mm_commands.d \
./Core/Src/mm_encoders.d \
./Core/Src/mm_floodfill.d \
./Core/Src/mm_motors.d \
./Core/Src/mm_profiles.d \
./Core/Src/mm_supplemental.d \
./Core/Src/mm_systick.d \
./Core/Src/mm_vision.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mm_commands.cyclo ./Core/Src/mm_commands.d ./Core/Src/mm_commands.o ./Core/Src/mm_commands.su ./Core/Src/mm_encoders.cyclo ./Core/Src/mm_encoders.d ./Core/Src/mm_encoders.o ./Core/Src/mm_encoders.su ./Core/Src/mm_floodfill.cyclo ./Core/Src/mm_floodfill.d ./Core/Src/mm_floodfill.o ./Core/Src/mm_floodfill.su ./Core/Src/mm_motors.cyclo ./Core/Src/mm_motors.d ./Core/Src/mm_motors.o ./Core/Src/mm_motors.su ./Core/Src/mm_profiles.cyclo ./Core/Src/mm_profiles.d ./Core/Src/mm_profiles.o ./Core/Src/mm_profiles.su ./Core/Src/mm_supplemental.cyclo ./Core/Src/mm_supplemental.d ./Core/Src/mm_supplemental.o ./Core/Src/mm_supplemental.su ./Core/Src/mm_systick.cyclo ./Core/Src/mm_systick.d ./Core/Src/mm_systick.o ./Core/Src/mm_systick.su ./Core/Src/mm_vision.cyclo ./Core/Src/mm_vision.d ./Core/Src/mm_vision.o ./Core/Src/mm_vision.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su

.PHONY: clean-Core-2f-Src

