CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
OBJCP := arm-none-eabi-objcopy
INC = -Iinclude -I../../os/arch/f401/include/
OUTDIR := ./build

CFLAGS = -O0  \
	 -g -mcpu=cortex-m4 -mthumb \
	 -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

TARGET = f401spi
all: $(TARGET)

$(TARGET): clean compile_c build_platform_f401 link convert_elf

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o $(OUTDIR)

build_platform_f401:
	make -C ../../os/arch/f401 HWFP=0
	mv ../../os/arch/f401/build/*.o ./build

link: 
	make -C ./linkscripts TARGET=$(TARGET)

convert_elf:
	$(OBJCP) $(OUTDIR)/$(TARGET) $(OUTDIR)/$(TARGET)_binary -O binary

clean:
	rm -f $(OUTDIR)/*.o ./build/*
