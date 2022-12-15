#!/bin/bash
make
/home/benni/development/nextlevel/nextleveleos-master/virt/kill.sh
cd ~/development/nextlevel/nextleveleos-master/build 
arm-none-eabi-objdump firmware.elf -S > dump
cd ~/development/nextlevel/nextleveleos-master/virt 
