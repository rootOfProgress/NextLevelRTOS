#!/bin/bash
#arm-none-eabi-gcc func.c -c -ffreestanding -nostdlib -fno-common -O2 -g -mcpu=cortex-m4 -mthumb
#arm-none-eabi-gcc -ffreestanding -nostdlib func.o -o tryasm -T link1.ld -g -mcpu=cortex-m4 -mthumb
#arm-none-eabi-objcopy tryasm -O binary task
arm-none-eabi-gcc -O0 -mcpu=cortex-m4 -mthumb -T link.ld -Wall -Wextra main.c -o subprog -g -nostdlib  -nostartfiles 
arm-none-eabi-objcopy subprog prog -O binary
cp subprog ~/development/os_dev/NextLevelRTOS/kernel
