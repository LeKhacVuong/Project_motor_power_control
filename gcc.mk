#TOOLCHAIN_ROOT:=/mnt/workspace/SeleX.JSC/Firmware/toolchain/gcc-arm-none-eabi-7-2017-q4-major/bin
#TOOLCHAIN_ROOT:=/mnt/workspace/SeleX.JSC/Firmware/toolchain/gcc-arm-none-eabi-7-2018-q2-update/bin
export
CCPREFIX  	:=arm-none-eabi-
CC      	= $(CCPREFIX)gcc
CP        	= $(CCPREFIX)objcopy
LD      	= ${CCPREFIX}ld
DUMP		= $(CCPREFIX)objdump
AS        	= $(CCPREFIX)gcc -x assembler-with-cpp
AR      	= $(CCPREFIX)ar
GDBTUI    	= $(CCPREFIX)gdbtui
SIZE	  	= $(CCPREFIX)size
HEX       	= $(CP) -O ihex
BIN       	= $(CP) -O binary
