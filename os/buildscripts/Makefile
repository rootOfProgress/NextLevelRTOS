CC := arm-none-eabi-gcc
LINK = ../arch/$(ARCH)/link.ld

LFLAGS = -nostdlib -nostartfiles
OBJ:=$(wildcard ../build/*.o)

TARGET = $(ARCH)

ifeq (1,$(HWFP))
all: link_hard
else
all: link_soft create_api
endif

link_hard: $(OBJ)
	@echo "Link hard fp ..."
	$(CC) -T $(LINK) \
	-L../lib/math/hardfp \
	-ffunction-sections $^  -o ../build/firmware $(LFLAGS) -lm 

create_api:
	python collect_external_headers.py $(ARCH)

link_soft: $(OBJ)
	@echo "Link soft fp ..."
	$(CC) -T $(LINK) \
	-L../lib/math/nofp \
	-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/12.2.0/thumb/v7e-m/nofp/ \
	-ffunction-sections $^  -o ../build/os.$(ARCH) $(LFLAGS) -Xlinker -Map=../build/memmap.$(ARCH) \
	-lm -lgcc \
	../../packages/lib/archives/*.a
