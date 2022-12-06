#!/bin/bash
#make
./qemu-system-arm -machine netduinoplus2 -kernel ../c_rework/build/firmware -nographic -cpu cortex-m4 -S -s
