#!/bin/bash
#arm-none-eabi-gcc func.c -c -ffreestanding -nostdlib -fno-common -O2 -g -mcpu=cortex-m4 -mthumb
#arm-none-eabi-gcc -ffreestanding -nostdlib func.o -o tryasm -T link1.ld -g -mcpu=cortex-m4 -mthumb
#arm-none-eabi-objcopy tryasm -O binary task
arm-none-eabi-gcc -O0 -mcpu=cortex-m4 -mthumb -Wall -Wextra main.c -c -g -nostdlib -nostartfiles 
arm-none-eabi-as lowlevel.s -o asmprog.o
arm-none-eabi-gcc -T link.ld asmprog.o main.o -o subprog -nostdlib -nostartfiles 

arm-none-eabi-objcopy subprog prog -O binary
cat zeros >> prog
cp subprog ~/development/os_dev/NextLevelRTOS/kernel
