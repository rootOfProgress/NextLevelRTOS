CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
OBJCP := arm-none-eabi-objcopy
INC = -Iinclude
LINK = ../link_external.ld
OUTDIR := ./build

CFLAGS = -O2  \
	 -g -mcpu=cortex-m4 -mthumb \
	 -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)
SRC_AS=$(shell find $('./src') -name '*.s')
TARGET = external

all: $(TARGET)

$(TARGET): clean compile_c link convert_elf

move_objects:
	mv *.o $(OUTDIR)

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o $(OUTDIR)

compile_as: $(SRC_AS)
	$(AS) $^ -o ./build/asm.o

link: 
	make -C $(OUTDIR)

convert_elf:
	$(OBJCP) $(OUTDIR)/$(TARGET) $(OUTDIR)/$(TARGET)_binary -O binary
	cat zeros >> $(OUTDIR)/$(TARGET)_binary
	cp $(OUTDIR)/$(TARGET)_binary ../task
clean:
	rm -f $(OUTDIR)/*.o $(TARGET)
