#!/bin/bash
dd if=./data of=/tmp/s9.in iflag=direct,skip_bytes
sleep 1
dd if=./prepare of=/tmp/s9.in iflag=direct,skip_bytes
sleep 1
dd if=./task of=/tmp/s9.in iflag=direct,skip_bytes
