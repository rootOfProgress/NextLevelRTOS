#!/bin/bash
#gdb-multiarch -q -x ./stm.gdb -se=../build/firmware.elf
arm-none-eabi-gdb -q -x ./stm.gdb -se=../build/firmware.elf
