CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
OBJCP := arm-none-eabi-objcopy
INC = -I../include
OUTDIR := ../commonbuild

CFLAGS = -O0  \
	 -g -mcpu=cortex-m4 -mthumb \
	 -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

SRC:=$(wildcard ./src/**/*.c) 
SRC+=$(wildcard ./src/*.c)

OBJ:=$(wildcard ./build/*.o)

TARGET = softcrc
all: $(TARGET)

$(TARGET): clean compile_c

compile_c: $(SRC)
	$(CC) $(CFLAGS) $(INC) -c $^ 
	mv *.o $(OUTDIR)

clean:
	rm -f $(OUTDIR)/*.o
