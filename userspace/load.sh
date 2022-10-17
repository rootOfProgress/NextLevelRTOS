#!/bin/bash
printf "f" >> /dev/ttyUSB0 
sleep 0.5
printf "T" >> /dev/ttyUSB0 
sleep 0.5
sudo dd if=./prog of=/dev/ttyUSB0 bs=512 iflag=direct,skip_bytes count=1