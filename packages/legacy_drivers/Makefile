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

$(TARGET): convert_elf


convert_elf:
	cd runtime && cargo b 2>/dev/null && cd ..
	$(OBJCP) runtime/target/thumbv7em-none-eabihf/debug/runtime ./build/legacy_drivers_binary -O binary
	#cat zeros >> legacy_bin
	#cp legacy_bin ../driver
	#truncate --size=512 ../driver

clean:
	rm -f $(OUTDIR)/*.o $(TARGET)
