#!/bin/bash
#make
./qemu-system-arm -machine netduinoplus2 -kernel ../kernel/target/thumbv7em-none-eabihf/debug/kernel -nographic -cpu cortex-m4 -S -s
