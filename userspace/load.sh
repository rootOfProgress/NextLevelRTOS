#!/bin/bash
sudo dd if=./prog of=/dev/ttyUSB0 bs=512 iflag=direct,skip_bytes count=1
