################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/EPD_SmallSized_Driver_HAL/Src/epd_driver.c \
../Drivers/EPD_SmallSized_Driver_HAL/Src/zipper_screen.c 

OBJS += \
./Drivers/EPD_SmallSized_Driver_HAL/Src/epd_driver.o \
./Drivers/EPD_SmallSized_Driver_HAL/Src/zipper_screen.o 

C_DEPS += \
./Drivers/EPD_SmallSized_Driver_HAL/Src/epd_driver.d \
./Drivers/EPD_SmallSized_Driver_HAL/Src/zipper_screen.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/EPD_SmallSized_Driver_HAL/Src/%.o Drivers/EPD_SmallSized_Driver_HAL/Src/%.su Drivers/EPD_SmallSized_Driver_HAL/Src/%.cyclo: ../Drivers/EPD_SmallSized_Driver_HAL/Src/%.c Drivers/EPD_SmallSized_Driver_HAL/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U031xx -c -I../Core/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U0xx/Include -I../Drivers/CMSIS/Include -I../Drivers/EPD_SmallSized_Driver_HAL/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-EPD_SmallSized_Driver_HAL-2f-Src

clean-Drivers-2f-EPD_SmallSized_Driver_HAL-2f-Src:
	-$(RM) ./Drivers/EPD_SmallSized_Driver_HAL/Src/epd_driver.cyclo ./Drivers/EPD_SmallSized_Driver_HAL/Src/epd_driver.d ./Drivers/EPD_SmallSized_Driver_HAL/Src/epd_driver.o ./Drivers/EPD_SmallSized_Driver_HAL/Src/epd_driver.su ./Drivers/EPD_SmallSized_Driver_HAL/Src/zipper_screen.cyclo ./Drivers/EPD_SmallSized_Driver_HAL/Src/zipper_screen.d ./Drivers/EPD_SmallSized_Driver_HAL/Src/zipper_screen.o ./Drivers/EPD_SmallSized_Driver_HAL/Src/zipper_screen.su

.PHONY: clean-Drivers-2f-EPD_SmallSized_Driver_HAL-2f-Src

