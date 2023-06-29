#!/bin/bash
max=10000000
for (( i=2; i <= $max; ++i ))
do                 
curl -X POST http://192.168.178.51:5000/upload/empty
sleep 1
done

