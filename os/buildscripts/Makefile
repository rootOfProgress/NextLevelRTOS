CC := arm-none-eabi-gcc
LINK = ../arch/$(ARCH)/link.ld

# CFLAGS = -O2  \
#          -mcpu=cortex-m4 -mthumb \
#          -g -nostartfiles -ffunction-sections  -Wall -Wextra -static -nostdlib  \

LFLAGS = -nostdlib -nostartfiles
OBJ:=$(wildcard ../build/*.o)

# ifeq (0,$(HWFP))
# LFLAGS += -lgcc
# endif

TARGET = $(ARCH)
ifeq (1,$(HWFP))
all: link_hard
else
all: link_soft
endif

# -L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/arm-none-eabi/lib/thumb/v7+fp/hard \
# -L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/12.2.0/thumb/v7+fp/hard \
	-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/arm-none-eabi/lib/thumb/v7e-m+fp/hard \
	-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/12.2.0/thumb/v7e-m+fp/hard \

link_hard: $(OBJ)
	$(CC) -T $(LINK) \
	-L../lib/math/hardfp \
	-ffunction-sections $^  -o ../build/firmware $(LFLAGS) -lm 
#// $((if (0,$(HWFP)), "-lgcc"))

# -L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/arm-none-eabi/lib/thumb/nofp/ \
# -L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/12.2.0/thumb/v7+fp/softfp/ \
	-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/arm-none-eabi/lib/thumb/v7e-m/nofp/ \
	-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/12.2.0/thumb/v7e-m+fp/softfp/ \
	-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/arm-none-eabi/lib/thumb/v7e-m/nofp/ \

#-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/12.2.0/thumb/v7e-m/nofp/ \
	-L../lib/gcc/nofp \

link_soft: $(OBJ)
	$(CC) -T $(LINK) \
	-L../lib/math/nofp \
	-L/usr/share/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/12.2.0/thumb/v7e-m/nofp/ \
	-ffunction-sections $^  -o ../build/firmware $(LFLAGS) -lm -lgcc
# $(CC) -T $(LINK)  -ffunction-sections $^  -o ../build/firmware $(LFLAGS) -lc -lm -lgcc
#// $((if (0,$(HWFP)), "-lgcc"))