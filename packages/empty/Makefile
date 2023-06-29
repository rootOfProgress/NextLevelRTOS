CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
OBJCP := arm-none-eabi-objcopy
INC = -I../include
OUTDIR := ./build

CFLAGS = -O0  \
	 -g -mcpu=cortex-m4 -mthumb \
	 -nostartfiles -ffunction-sections  -Wall -Wextra -mfloat-abi=soft -static -nostdlib  \

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

TARGET := empty
all: $(TARGET)

$(TARGET): clean compile_c link convert_elf

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o $(OUTDIR)

link: 
	make -C $(OUTDIR)

convert_elf:
	$(OBJCP) $(OUTDIR)/$(TARGET) $(OUTDIR)/$(TARGET)_binary -O binary

clean:
	rm -f $(OUTDIR)/*.o ./build/$(TARGET)*
