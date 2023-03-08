CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
OBJCP := arm-none-eabi-objcopy
INC = -Iinclude
OUTDIR := ./build

CFLAGS = -O0  \
	 -g -mcpu=cortex-m4 -mthumb \
	 -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

TARGET = f401spi
all: $(TARGET)

$(TARGET): clean compile_c link convert_elf

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o $(OUTDIR)

link: 
	make -C ./linkscripts TARGET=$(TARGET)

convert_elf:
	$(OBJCP) $(OUTDIR)/$(TARGET) $(OUTDIR)/$(TARGET)_binary -O binary

clean:
	rm -f $(OUTDIR)/*.o ./build/*
