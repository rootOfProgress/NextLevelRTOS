CC := arm-none-eabi-gcc
LINK = ../link_external.ld

OBJ:=$(wildcard ../build/*.o)

all: link

link: $(OBJ)
	$(CC) -T $(LINK) \
	-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/arm-none-eabi/lib/thumb/v7e-m/nofp \
	-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/12.2.0/thumb/v7e-m/nofp/ \
    $^ -o ../build/$(TARGET) -nostdlib -nostartfiles -lm -lgcc
