################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/src/stm32f407xx_gpio._driver.c \
../Drivers/src/stm32f407xx_i2c_driver.c \
../Drivers/src/stm32f407xx_rcc_driver.c \
../Drivers/src/stm32f407xx_spi_driver.c \
../Drivers/src/stm32f407xx_usart_driver.c 

OBJS += \
./Drivers/src/stm32f407xx_gpio._driver.o \
./Drivers/src/stm32f407xx_i2c_driver.o \
./Drivers/src/stm32f407xx_rcc_driver.o \
./Drivers/src/stm32f407xx_spi_driver.o \
./Drivers/src/stm32f407xx_usart_driver.o 

C_DEPS += \
./Drivers/src/stm32f407xx_gpio._driver.d \
./Drivers/src/stm32f407xx_i2c_driver.d \
./Drivers/src/stm32f407xx_rcc_driver.d \
./Drivers/src/stm32f407xx_spi_driver.d \
./Drivers/src/stm32f407xx_usart_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/src/%.o Drivers/src/%.su Drivers/src/%.cyclo: ../Drivers/src/%.c Drivers/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I"D:/stm32_driver_development/stm32f4xx_drivers/Drivers/inc" -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-src

clean-Drivers-2f-src:
	-$(RM) ./Drivers/src/stm32f407xx_gpio._driver.cyclo ./Drivers/src/stm32f407xx_gpio._driver.d ./Drivers/src/stm32f407xx_gpio._driver.o ./Drivers/src/stm32f407xx_gpio._driver.su ./Drivers/src/stm32f407xx_i2c_driver.cyclo ./Drivers/src/stm32f407xx_i2c_driver.d ./Drivers/src/stm32f407xx_i2c_driver.o ./Drivers/src/stm32f407xx_i2c_driver.su ./Drivers/src/stm32f407xx_rcc_driver.cyclo ./Drivers/src/stm32f407xx_rcc_driver.d ./Drivers/src/stm32f407xx_rcc_driver.o ./Drivers/src/stm32f407xx_rcc_driver.su ./Drivers/src/stm32f407xx_spi_driver.cyclo ./Drivers/src/stm32f407xx_spi_driver.d ./Drivers/src/stm32f407xx_spi_driver.o ./Drivers/src/stm32f407xx_spi_driver.su ./Drivers/src/stm32f407xx_usart_driver.cyclo ./Drivers/src/stm32f407xx_usart_driver.d ./Drivers/src/stm32f407xx_usart_driver.o ./Drivers/src/stm32f407xx_usart_driver.su

.PHONY: clean-Drivers-2f-src

