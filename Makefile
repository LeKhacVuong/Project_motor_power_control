# Prefix for the arm-eabi-none toolchain.
# I'm using codesourcery g++ lite compilers available here:
# http://www.mentor.com/embedded-software/sourcery-tools/sourcery-codebench/editions/lite-edition/
# Microcontroller properties.

export TARGET=bss_master
export PROJ_ROOT=.
export BOARD_TYPE=stm32_rs485_bsp
include $(PROJ_ROOT)/board/$(BOARD_TYPE)/$(BOARD_TYPE).mk
include $(PROJ_ROOT)/gcc.mk

SRCS:=board/board.c

SRCS+=service/host_master/host_master.c
SRCS+=service/sm_modbus_master.c	
SRCS+=libs/nanoModbus/nanomodbus.c	
	
SRCS+=util/string/string_util.c \
	util/crc/crc.c
	
BSP_SRCS:=$(addprefix board/$(BOARD_TYPE)/,$(BSP_SRCS))
BSP_INCLUDES:=$(addprefix board/$(BOARD_TYPE)/,$(BSP_INCLUDES))

#INCLUDES:=. app_config board service component util app
INCLUDES:=app/cabinet_app \

INCLUDES+=service
INCLUDES+=service/host_master
             
	
INCLUDES+= util/string \
	util/crc

INCLUDES+= board
INCLUDES+= libs

USER_LIB_INCLUDES=$(PROJ_ROOT)/libs/selex-libc/canopen


LIB_INCLUDES+=
OBJDIR=build

INCLUDES+=$(LIB_INCLUDES)			
INCLUDES+=$(BSP_INCLUDES)
INCLUDES:=$(addprefix -I$(PROJ_ROOT)/,$(INCLUDES))
INCLUDES+=$(addprefix -I,$(USER_LIB_INCLUDES))

SRCS+=$(BSP_SRCS)
	
OBJS:=$(addprefix $(PROJ_ROOT)/$(OBJDIR)/,$(SRCS))		
SRCS:=$(addprefix $(PROJ_ROOT)/,$(SRCS))

SRCS+=$(PROJ_ROOT)/main.c
OBJS+=$(PROJ_ROOT)/$(OBJDIR)/main.c


OBJS:= $(patsubst %.c,%.o,$(OBJS))
OBJS:= $(patsubst %.s,%.o,$(OBJS))
DEPS:= $(patsubst %.o,%.d,$(OBJS))

LDSCRIPT_INC=
DEFS:=

#--------------------------------------------------------

#OPTIMIZE=-O0
OPTIMIZE=-O0

# Option arguments for C compiler.
CFLAGS+= $(INCLUDES)
CFLAGS+= $(OPTIMIZE)
CFLAGS+=-fmessage-length=0
CFLAGS+=-fsigned-char
CFLAGS+=-Wall -Winline -ggdb -lm -MMD -MP -Wno-unused-function -Wextra -Wstrict-prototypes
CFLAGS+=-std=gnu11
CFLAGS+=--specs=nano.specs
CFLAGS+=-ffunction-sections -fdata-sections

LFLAGS:=-T$(PROJ_ROOT)/board/$(BOARD_TYPE)/$(LD_FILES)
#LFLAGS	+=-nostartfiles -Xlinker --gc-sections
LFLAGS	+=-Xlinker --gc-sections
LFLAGS  +=-Wl,-Map=$(TARGET).map

LINK_LIBS:= $(addprefix -L,$(USER_LIB_INCLUDES))
LINK_LIBS+= $(addprefix -l,$(USER_LIBS))

#LFLAGS  += -flto -fuse-linker-plugin
# Flags for objcopy
CPFLAGS = -Obinary

# flags for objectdump
ODFLAGS = -S

# Uploader tool path.
FLASHER=JFlashLite
JLINKGDB=JLinkGDBServer
# Flags for the uploader program.
FLASHER_FLAGS=

###################################################

.PHONY:all proj debug libs test_suit test_suit_clean flash

all: proj
	$(HEX)   $(TARGET).elf 	$(TARGET).hex
	$(BIN)   $(TARGET).elf  $(TARGET).bin
	$(DUMP) -St $(TARGET).elf >$(TARGET).lst
	$(SIZE)  $(OBJS) $(TARGET).elf $(TARGET).hex

proj: 	$(TARGET).elf

-include $(DEPS)
$(PROJ_ROOT)/$(OBJDIR)/%.o: $(PROJ_ROOT)/%.c
	@echo Compiling $<...
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) ${<} -o ${@}

$(PROJ_ROOT)/$(OBJDIR)/%.o: $(PROJ_ROOT)/%.s
	@echo Compiling $<...
	@mkdir -p $(dir $@)
	$(AS) -c $(CFLAGS) $< -o $@

$(TARGET).elf: $(OBJS)
	@echo Linking...
	$(CC) $(CFLAGS) $(LFLAGS) -o ${TARGET}.elf $(OBJS) $(LINK_LIBS)

clean:
	find ./ -name '*~' | xargs rm -f	
	rm -f *.o
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f $(OBJDIR)/*.o
	rm -f $(OBJDIR)/*.d
	rm -f $(TARGET).elf
	rm -f $(TARGET).hex
	rm -f $(TARGET).bin
	rm -f $(TARGET).map
	rm -f $(TARGET).lst

libs:
	make -C libs/selex-libc/canopen all
test_suit:
	make -C libs/selex-libc/test-suit/ -f test_suit.mk all
test_suit_clean:
	make -C libs/selex-libc/test-suit/ -f test_suit.mk clean
	
flash:
	./flasher.sh
