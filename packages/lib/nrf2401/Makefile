CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
OBJCP := arm-none-eabi-objcopy
INC = -I../include -I../../include -I../../../os/arch/f401/include/
OUTDIR := ./build

CFLAGS = -O0  \
	 -g -mcpu=cortex-m4 -mthumb \
	 -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

SRC:=$(wildcard ./src/**/*.c) $(wildcard ./src/*.c)
OBJ:=$(wildcard ./build/*.o)

TARGET = nrf2401
all: $(TARGET)

$(TARGET): clean compile_c build_platform_f401 archive 

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o $(OUTDIR)

build_platform_f401:
	make -C ../../../os/arch/f401 HWFP=0
	mv ../../../os/arch/f401/build/*.o ./build

archive: 
	ar -rc ../archives/lib$(TARGET).a build/*.o

clean:
	rm -f $(OUTDIR)/*.o ./build/*
