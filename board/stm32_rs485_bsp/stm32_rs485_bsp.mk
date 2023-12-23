BSP_SRCS=  \
	   Core/Src/core.c \
	Core/Src/sysmem.c \
	Core/Src/syscalls.c \
	Core/Src/system_stm32f1xx.c \
	Core/Src/stm32f1xx_it.c \
	Core/Src/stm32f1xx_hal_msp.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_usart.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_iwdg.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c  

#BSP_SRCS+=sdk/CMSIS/CM3/CoreSupport/core_cm3.c

BSP_SRCS+=$(BOARD_TYPE).c  \
	uart_hw/uart_hw.c \
	can_hardware/can_hardware.c \
	core_hw/core_hw.c 

# ASM sources
BSP_ASM_SRCS = Core/Startup/startup_stm32f103rbtx.s
BSP_SRCS+=$(BSP_ASM_SRCS)

# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi
FLOAT-ABI=-mfloat-abi=soft

# mcu
CFLAGS+= $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
ASFLAGS+= $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =-DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB

# C includes
BSP_INCLUDES = . uart_hw \
	can_hardware \
	core_hw \
	Core/Inc \
	Drivers/STM32F1xx_HAL_Driver/Inc  \
	Drivers/STM32F1xx_HAL_Driver/Inc/Legacy  \
	Drivers/CMSIS/Device/ST/STM32F1xx/Include \
	Drivers/CMSIS/Include 

# compile gcc flags
ASFLAGS+= $(AS_DEFS) $(AS_INCLUDES)

CFLAGS+= $(C_DEFS) $(C_INCLUDES)

#######################################
# LDFLAGS
#######################################
# link script
LD_FILES:=STM32F103RBTX_FLASH.ld
