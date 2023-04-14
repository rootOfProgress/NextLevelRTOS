#!/bin/bash

while [ 1 ]
do
    curl -X GET 192.168.178.51:5000/position
    #clear
    sleep 0.3
done
