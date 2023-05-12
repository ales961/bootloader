################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/uart/buffer.c \
../Core/Src/uart/command.c \
../Core/Src/uart/menu.c \
../Core/Src/uart/uart.c \
../Core/Src/uart/xmodem.c 

OBJS += \
./Core/Src/uart/buffer.o \
./Core/Src/uart/command.o \
./Core/Src/uart/menu.o \
./Core/Src/uart/uart.o \
./Core/Src/uart/xmodem.o 

C_DEPS += \
./Core/Src/uart/buffer.d \
./Core/Src/uart/command.d \
./Core/Src/uart/menu.d \
./Core/Src/uart/uart.d \
./Core/Src/uart/xmodem.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/uart/%.o Core/Src/uart/%.su: ../Core/Src/uart/%.c Core/Src/uart/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F427xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../LWIP/App -I../LWIP/Target -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/BSP/Components/lan8742 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-uart

clean-Core-2f-Src-2f-uart:
	-$(RM) ./Core/Src/uart/buffer.d ./Core/Src/uart/buffer.o ./Core/Src/uart/buffer.su ./Core/Src/uart/command.d ./Core/Src/uart/command.o ./Core/Src/uart/command.su ./Core/Src/uart/menu.d ./Core/Src/uart/menu.o ./Core/Src/uart/menu.su ./Core/Src/uart/uart.d ./Core/Src/uart/uart.o ./Core/Src/uart/uart.su ./Core/Src/uart/xmodem.d ./Core/Src/uart/xmodem.o ./Core/Src/uart/xmodem.su

.PHONY: clean-Core-2f-Src-2f-uart

