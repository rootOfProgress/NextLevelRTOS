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

#while [ 1 ]
#do
#printf "\\x01\\x12\\x34\\x56" >> /tmp/s9.out 
#dd if=./prepare of=/tmp/s9.out iflag=direct,skip_bytes
#dd if=./task of=/tmp/s9.out iflag=direct,skip_bytes
#done

while [ 1 ]
do
printf "\\x01\\x12\\x34\\x56" >> /dev/ttyUSB0 
dd if=./prepare of=/dev/ttyUSB0 iflag=direct,skip_bytes
dd if=./task of=/dev/ttyUSB0 iflag=direct,skip_bytes
done