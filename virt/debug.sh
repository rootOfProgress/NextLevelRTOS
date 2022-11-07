#!/bin/bash

arm-none-eabi-gdb -se=../kernel/target/thumbv7em-none-eabihf/debug/kernel -x ./stm.gdb
