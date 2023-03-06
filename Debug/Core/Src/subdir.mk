################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/boot_config.c \
../Core/Src/buffer.c \
../Core/Src/command.c \
../Core/Src/flash.c \
../Core/Src/gpio.c \
../Core/Src/hex_parser.c \
../Core/Src/iwdg.c \
../Core/Src/lwip_callbacks.c \
../Core/Src/main.c \
../Core/Src/menu.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/tim.c \
../Core/Src/uart.c \
../Core/Src/usart.c \
../Core/Src/xmodem.c 

OBJS += \
./Core/Src/boot_config.o \
./Core/Src/buffer.o \
./Core/Src/command.o \
./Core/Src/flash.o \
./Core/Src/gpio.o \
./Core/Src/hex_parser.o \
./Core/Src/iwdg.o \
./Core/Src/lwip_callbacks.o \
./Core/Src/main.o \
./Core/Src/menu.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/tim.o \
./Core/Src/uart.o \
./Core/Src/usart.o \
./Core/Src/xmodem.o 

C_DEPS += \
./Core/Src/boot_config.d \
./Core/Src/buffer.d \
./Core/Src/command.d \
./Core/Src/flash.d \
./Core/Src/gpio.d \
./Core/Src/hex_parser.d \
./Core/Src/iwdg.d \
./Core/Src/lwip_callbacks.d \
./Core/Src/main.d \
./Core/Src/menu.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/tim.d \
./Core/Src/uart.d \
./Core/Src/usart.d \
./Core/Src/xmodem.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F427xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../LWIP/App -I../LWIP/Target -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/BSP/Components/lan8742 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/boot_config.d ./Core/Src/boot_config.o ./Core/Src/boot_config.su ./Core/Src/buffer.d ./Core/Src/buffer.o ./Core/Src/buffer.su ./Core/Src/command.d ./Core/Src/command.o ./Core/Src/command.su ./Core/Src/flash.d ./Core/Src/flash.o ./Core/Src/flash.su ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/hex_parser.d ./Core/Src/hex_parser.o ./Core/Src/hex_parser.su ./Core/Src/iwdg.d ./Core/Src/iwdg.o ./Core/Src/iwdg.su ./Core/Src/lwip_callbacks.d ./Core/Src/lwip_callbacks.o ./Core/Src/lwip_callbacks.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/menu.d ./Core/Src/menu.o ./Core/Src/menu.su ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/uart.d ./Core/Src/uart.o ./Core/Src/uart.su ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su ./Core/Src/xmodem.d ./Core/Src/xmodem.o ./Core/Src/xmodem.su

.PHONY: clean-Core-2f-Src

