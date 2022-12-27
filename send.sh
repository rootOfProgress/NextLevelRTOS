#!/bin/bash
# for i in {0..10}
# do
#    echo "go.."
#    dd if=./data of=/tmp/s9.in iflag=direct,skip_bytes
# #    sleep 1
#    dd if=./prepare of=/tmp/s9.in iflag=direct,skip_bytes
# #    sleep 1
#    dd if=./task of=/tmp/s9.in iflag=direct,skip_bytes
#    sleep 0.1
# done
#printf "\\x01\\x12\\x34\\x56" >> /dev/ttyUSB0 
#sync
#dd if=./prepare of=/dev/ttyUSB0 iflag=direct,skip_bytes
#sync
#dd if=./task of=/dev/ttyUSB0 iflag=direct,skip_bytes
#sync

if [[ $1 -eq "1" ]]
then 
    printf "\\x01\\x12\\x34\\x56" >> /dev/ttyUSB0 
    sleep 0.5
    dd if=./legay_drivers/prepareDriver of=/dev/ttyUSB0 iflag=direct,skip_bytes
    sleep 0.5
    dd if=$2 of=/dev/ttyUSB0 iflag=direct,skip_bytes
elif [[ $1 -eq "2" ]]
then
    printf "\\x01\\x12\\x34\\x56" >> /tmp/s9.in 
    sleep 0.5
    dd if=./prepareQemu of=/tmp/s9.in iflag=direct,skip_bytes
    sleep 0.5
    dd if=$2 of=/tmp/s9.in iflag=direct,skip_bytes
fi
