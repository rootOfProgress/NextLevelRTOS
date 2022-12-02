#!/bin/bash
#gdb-multiarch -q -x ./stm.gdb -se=../build/firmware.elf
arm-none-eabi-gdb -q -x ./stm.gdb -se=/home/benni/development/os_dev/NextLevelRTOS/c_rework/build/firmware
