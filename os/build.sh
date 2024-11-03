#!/bin/bash
source buildscripts/venv/bin/activate        
make ARCH=$1 SYSTICK=0 DEBUG=1 HWFP=0

