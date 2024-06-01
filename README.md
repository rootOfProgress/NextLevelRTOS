# NextLevelRTOS

[![Cortex M Types](https://img.shields.io/badge/target-thumbv7em--none--eabihf-green)](https://docs.rust-embedded.org/cortex-m-quickstart/cortex_m_quickstart/) 

[![Status](https://img.shields.io/badge/Status-W.I.P-red)]()

## Description
A compact embedded operating system with the following features:

## Implemented Features
* FiFo Scheduling: Supports scheduling of up to N tasks.
* User-/Kernelspace Separation: Utilizes Cortex M4's Handler-/Threadmode feature for basic separation between user and kernel space.
* Dynamic Memory Allocation: Provides basic capabilities for dynamic memory allocation.
* Thread Join: Supports the ability for threads to join.
* Thread Sleep: Allows threads to sleep.
* Dynamic Task Execution: Enables dynamic execution of tasks by sending compiled binaries during runtime via UART for scheduling.

## Known Issues
* The system is currently not HAL compatible; however, some boards are supported in the /arch directory.
* Scheduler does not work if preemptive scheduling is required, i.e. no systick context switching.
* Seperation of os code and architecture specific code must be improved.