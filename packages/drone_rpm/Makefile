CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
OBJCP := arm-none-eabi-objcopy
INC = -I./include -I../include -I../../os/arch/f401/include/ -I../lib/nrf2401/include
OUTDIR := ./build

CFLAGS = -O0  \
	 -g -mcpu=cortex-m4 -mthumb \
	 -nostartfiles -ffunction-sections  -Wall -Wextra -mfloat-abi=soft -static -nostdlib  \

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

TARGET := drone_rpm
all: $(TARGET)

$(TARGET): clean compile_c build_platform_f401 link convert_elf 

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o $(OUTDIR)

build_platform_f401:
	make -C ../../os/arch/f401 HWFP=0
	mv ../../os/arch/f401/build/*.o ./build

link: 
	make -C $(OUTDIR)

convert_elf:
	$(OBJCP) $(OUTDIR)/$(TARGET) $(OUTDIR)/$(TARGET)_binary -O binary

clean:
	rm -f $(OUTDIR)/*.o ./build/$(TARGET)*
