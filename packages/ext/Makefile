CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
OBJCP := arm-none-eabi-objcopy
INC = -Iinclude
OUTDIR := ./build

CFLAGS = -O2  \
	 -g -mcpu=cortex-m4 -mthumb \
	 -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

all: measure

measure: clean compile_c link convert_elf

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o $(OUTDIR)

link: 
	make -C $(OUTDIR)

convert_elf:
	$(OBJCP) $(OUTDIR)/measure $(OUTDIR)/measure_binary -O binary

clean:
	rm -f $(OUTDIR)/*.o measure