#!/bin/bash
#make
./qemu-system-arm -machine netduinoplus2 -kernel ../build/firmware.elf -nographic -cpu cortex-m4 -S -s
