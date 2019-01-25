

#C_SOURCES = uart.c  wiring_pulse.c i2c.c wiring_analog.c i2c.h wiring.c wiring_digital.c
#CPP_SOURCES = CMPS03.cpp main.cpp Print.cpp WString.cpp Goldboard4.cpp Motor.cpp Serial.cpp new.cpp PCF8574A.cpp SRF08.cpp

INC = -I. -Igoldboard

#VPATH = goldboard

MCU     = atmega32
OSC     = 16000000UL
PROJECT = main

# ----- These configurations are quite likely not to be changed -----

# Binaries
GCC     = avr-gcc
G++     = avr-g++
RM      = rm -f
AVRDUDE = avrdude

# Files
EXT_C   = c
EXT_C++ = cpp
EXT_ASM = asm

# ----- No changes should be necessary below this line -----

OBJECTS = \
	$(patsubst %.$(EXT_C),%.o,$(wildcard *.$(EXT_C))) \
	$(patsubst %.$(EXT_C++),%.o,$(wildcard *.$(EXT_C++))) \
	$(patsubst %.$(EXT_ASM),%.o,$(wildcard *.$(EXT_ASM))) \
	$(patsubst %.$(EXT_C),%.o,$(wildcard goldboard/*.$(EXT_C))) \
	$(patsubst %.$(EXT_C++),%.o,$(wildcard goldboard/*.$(EXT_C++))) \
	$(patsubst %.$(EXT_ASM),%.o,$(wildcard goldboard/*.$(EXT_ASM))) \


CFLAGS = $(INC)
CFLAGS += -Os -std=gnu11 -ffunction-sections #-fdata-sections
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -DF_CPU=$(OSC)
CFLAGS += -mmcu=$(MCU)

C++FLAGS = $(INC)
C++FLAGS += -Os -std=gnu++11 -ffunction-sections# -fdata-sections
C++FLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
C++FLAGS += -Wall
C++FLAGS += -DF_CPU=$(OSC)
C++FLAGS += -mmcu=$(MCU)

ASMFLAGS = $(INC)
ASMFLAGS += -Os
ASMFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
ASMFLAGS += -Wall -Wstrict-prototypes
ASMFLAGS += -DF_CPU=$(OSC)
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -mmcu=$(MCU)

LDFLAGS = -Wl,-gc-sections

all: $(PROJECT).elf
	avr-objcopy -j .text -j .data -O ihex $(PROJECT).elf main.hex
	avr-size --mcu=atmega32 -C main.elf

%.elf: $(OBJECTS)
	$(G++) $(C++FLAGS) $(OBJECTS) --output $@ $(LDFLAGS)


%.o : %.$(EXT_C)
	$(GCC) $< $(CFLAGS) -c -o $@

%.o : %.$(EXT_C++)
	$(G++) $< $(C++FLAGS) -c -o $@

%.o : %.$(EXT_ASM)
	$(G++) $< $(ASMFLAGS) -c -o $@


clean:
	$(RM) $(PROJECT).elf $(OBJECTS) $(PROJECT).hex
	rm -r -f doc
	rm -r -f Debug
	rm -r -f Release

help:
	@echo "usage:"
	@echo "  make <target>"
	@echo ""
	@echo "targets:"
	@echo "  clean     Remove any non-source files"
	@echo "  config    Shows the current configuration"
	@echo "  help      Shows this help"
	@echo "  show-mcu  Show list of all possible MCUs"

config:
	@echo "configuration:"
	@echo ""
	@echo "Binaries for:"
	@echo "  C compiler:   $(GCC)"
	@echo "  C++ compiler: $(G++)"
	@echo "  Programmer:   $(AVRDUDE)"
	@echo "  remove file:  $(RM)"
	@echo ""
	@echo "Hardware settings:"
	@echo "  MCU: $(MCU)"
	@echo "  OSC: $(OSC)"
	@echo ""
	@echo "Defaults:"
	@echo "  C-files:   *.$(EXT_C)"
	@echo "  C++-files: *.$(EXT_C++)"
	@echo "  ASM-files: *.$(EXT_ASM)"

show-mcu:
	$(G++) --help=target

program: all
	$(AVRDUDE) -pm32 -Pusb -cavrispmkII -u -U flash:w:main.hex  -B1

fuse:
	$(AVRDUDE) -pm32 -Pusb -cavrispmkII -u -U lfuse:w:0xff:m -U hfuse:w:0xde:m -B100

doc:
	rm -r -f doc
	doxygen
