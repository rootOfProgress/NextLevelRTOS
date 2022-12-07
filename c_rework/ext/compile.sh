#!/bin/bash
#arm-none-eabi-gcc func.c -c -ffreestanding -nostdlib -fno-common -O2 -g -mcpu=cortex-m4 -mthumb
#arm-none-eabi-gcc -ffreestanding -nostdlib func.o -o tryasm -T link1.ld -g -mcpu=cortex-m4 -mthumb
#arm-none-eabi-objcopy tryasm -O binary task
arm-none-eabi-gcc -O0 -mcpu=cortex-m4 -mthumb -Wall -Wextra main.c -c -g -nostdlib -nostartfiles 
arm-none-eabi-gcc -T link_external.ld  main.o -o extprog -nostdlib -nostartfiles 

arm-none-eabi-objcopy extprog prog -O binary
cat zeros >> prog
cp prog ../task
#cp subprog ~/development/os_dev/NextLevelRTOS/kernel
