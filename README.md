# NextLevelRTOS

[![Cortex M Types](https://img.shields.io/badge/target-thumbv7em--none--eabihf-green)](https://docs.rust-embedded.org/cortex-m-quickstart/cortex_m_quickstart/) 

[![Status](https://img.shields.io/badge/Status-W.I.P-red)]()


## Description
This is a minimal multithreaded realtime operating system for the ARM CortexM4 processor without unnecessary libraries. The final goal is to compile your own OS with only the features you need - no boilerplate code, no libraries which inflate your binary without knowing exactly what they actually do. 
Currently this system is developed with a STM32F303 Discovery board as developing platform, but in future releases it will be running on a STM32F407VET6 board.
One benefit is that the system is mostly written in rust, so feel free to contribute for steps towards the future!

*NEW:* This repository acts also as a tutorial how to build such a system. Each branch is build on top of the other to help to understand the whole process. There is also a documentation sheet for each chapter to give more insights in this work. 

## Current Progress

### Implemented Features
* Multithreaded Round Robin Scheduling up to N Tasks
* Basic access to GPIO Device
* Basic UART setup to print information on a host terminal
* Basic User-/Kernelspace separating using Cortex M4 Handler-/Threadmode feature
* Suspend tasks for an amount of time under realtime conditions (ongoing)

### Open
* Dynamically feed system with precompiled ELF Files / User Programms
* UART Serial print with help of DMA
* UART Receive
* Scheduler modes : Realtime, Roundrobin, ... chooseable during runtime
* Compile Kernel for different platforms
* Better user/kernel separation
* Improve device support (I2C, more GPIO, ...)
* rtosHealth GUI for monitoring device status and interaction. Will be
developed with QT.