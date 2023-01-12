#!/bin/bash
DIR="$( cd "$( dirname "$0" )" && pwd )"
echo $DIR
if [[ $1 -eq "1" ]]
then 
    printf "\\x01\\x12\\x34\\x56" >> /dev/ttyUSB0 
    sleep 0.5
    dd if=$DIR/prepare of=/dev/ttyUSB0 iflag=direct,skip_bytes
    sleep 0.5
    #dd if=$DIR/build/measure_binary of=/dev/ttyUSB0 iflag=direct,skip_bytes
elif [[ $1 -eq "2" ]]
then
    printf "\\x01\\x12\\x34\\x56" >> /tmp/s9.in 
    sleep 0.5
    dd if=./prepareDriver of=/tmp/s9.in iflag=direct,skip_bytes
    sleep 0.5
    dd if=./legacy_bin of=/tmp/s9.in iflag=direct,skip_bytes
fi
