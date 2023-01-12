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

all: ext

ext: clean compile_c link convert_elf

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o $(OUTDIR)

link: 
	make -C $(OUTDIR)

convert_elf:
	$(OBJCP) $(OUTDIR)/ext $(OUTDIR)/ext_binary -O binary

clean:
	rm -f $(OUTDIR)/*.o ext
