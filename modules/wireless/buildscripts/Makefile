CC := arm-none-eabi-gcc
LINK = ../arch/$(ARCH)/link.ld
LFLAGS = -nostdlib -nostartfiles
OBJ:=$(wildcard ../build/*.o)

ifeq (1,$(HWFP))
all: link_hard
else
all: link_soft 
endif

link_hard: $(OBJ)
	$(CC) -T $(LINK) \
	-L../lib/math/hardfp \
	-ffunction-sections ../build/$^  -o ../build/firmware $(LFLAGS) -lm 

link_soft: $(OBJ)
	$(CC) -T ../link_external.ld \
	-L../lib/math/nofp \
	-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/12.2.0/thumb/v7e-m/nofp/ \
	-ffunction-sections ../build/$^ -o ../build/dummy $(LFLAGS) -lm -lgcc