
########### Application ################

TARGET := loolou
SRCS   := $(wildcard *.c)
INCS   := .

########### Microcontroller ############

MCU    := attiny2313
F_CPU  := 2000000
FORMAT := ihex

########### Compiler ###################

CC      := /usr/bin/avr-gcc
CFLAGS  :=
LDFLAGS :=

CSTANDARD := -std=gnu99

OBJCOPY := /usr/bin/avr-objcopy
OBJDUMP := /usr/bin/avr-objdump
SIZE    := /usr/bin/avr-size
NM      := /usr/bin/avr-nm
AVRDUDE := /usr/bin/avrdude

########### Programmer #################

AVRDUDE_PROGRAMMER = avrispv2
AVRDUDE_PORT = /dev/ttyACM0

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_FLASH = -U eeprom:w:$(TARGET).eep

AVRDUDE_FLAGS := -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)

########### Library ####################

# Minimalistic printf version
PRINTF_LIB_MIN := -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT := -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB :=
#PRINTF_LIB := $(PRINTF_LIB_MIN)
#PRINTF_LIB := $(PRINTF_LIB_FLOAT)

# Minimalistic scanf version
SCANF_LIB_MIN := -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT := -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB :=
#SCANF_LIB := $(SCANF_LIB_MIN)
#SCANF_LIB := $(SCANF_LIB_FLOAT)

MATH_LIB := -lm

########### Assembler #################

#ASFLAGS := -Wa,-adhlns=$(<:.S=.lst),-gstabs 
ASFLAGS += -DF_OSC=$(F_CPU)

########### Compiler (II) #############

CFLAGS  += -gdwarf-2
CFLAGS  += -DF_CPU=$(F_CPU)UL
CFLAGS  += -O3
#CFLAGS += -mint8
#CFLAGS += -mshort-calls
CFLAGS  += -funsigned-char
CFLAGS  += -funsigned-bitfields
CFLAGS  += -fpack-struct
CFLAGS  += -fshort-enums
#CFLAGS += -fno-unit-at-a-time
CFLAGS  += -Wall
CFLAGS  += -Wstrict-prototypes
CFLAGS  += -Wundef
#CFLAGS += -Wunreachable-code
#CFLAGS += -Wsign-compare
#CFLAGS += -Wa,-adhlns=$(<:%.c=$(OBJDIR)/%.lst)
CFLAGS  += -mmcu=$(MCU)

LDFLAGS += -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)

ASFLAGS  += -mmcu=$(MCU) -x assembler-with-cpp

########### Compiler ###################

OBJS := $(SRCS:%.c=%.o)
LSTS := $(SRCS:%.c=%.lst)

CFLAGS += $(CSTANDARD)
CFLAGS += $(patsubst %,-I%,$(INCS))

DEPDIR   = .dep
DEPFLAGS = -MD -MP -MF $(DEPDIR)/$(@F).d

TARGET_LIST := $(TARGET).elf $(TARGET).eep $(TARGET).lss $(TARGET).sym $(TARGET).hex

############### info ###################

$(info )
$(info +++++++ INFO +++++++)
$(info SRCS = $(SRCS))
$(info OBJS = $(OBJS))
$(info LSTS = $(LSTS))
$(info )
$(info TARGET_LIST = $(TARGET_LIST))
$(info ++++++++++++++++++++)
$(info )

############### main ###################

all: prepare $(TARGET_LIST) size

############### help ###################

help:
	@echo "make help   print this text"
	@echo "make all    compile all"
	@echo "make clean  remove all files"

############### flashing ###############

burn: all
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

############### compiling ##############

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.SECONDARY:
%.o : %.c
	$(CC) -c $(CFLAGS) $(DEPFLAGS) $< -o $@

%.elf: $(OBJS)
	$(CC) $(CFLAGS) $^ --output $@ $(LDFLAGS)

%.eep: $(TARGET).elf
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

%.lss: %.elf
	$(OBJDUMP) -h -S $< > $@

%.sym: %.elf
	$(NM) -n $< > $@

size:
	@echo ""
	$(SIZE) --target=$(FORMAT) $(TARGET).hex
	$(SIZE) -A $(TARGET).elf
	@echo ""

prepare:
	@mkdir -p .dep

clean:
	rm -rf $(OBJS) $(TARGET_LIST) $(TARGET).map $(DEPDIR) $(OBJDIR)
