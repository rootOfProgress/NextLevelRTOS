# Device Access

## What you will learn
* How to access, read and write to registers 
* Binary Operations
* Configure GPIO Ports as output
* Rust Module System
* Get a "real world" feedback from the hardware

## Goals

This chapters has two targets. First it should bring you in touch which General Purpose Input Output pins, **GPIO** in shorthand, their use and their configuration. Second you get more knowledge about the rust module system. We will use it to prepare the classification of the devices for future chapters.

## Prepare the environment

As written in the main readme file we wanna design a operating system that may fit for different platforms, as long as the main cpu is a cortex m4. Different devices on any board have different adresses which purpose are to access them. One way could be to hardcode every adress into your source code - but if you change the platform it would be a big mess to change every single adress. We will centralize this information into a module, so that we can simply exchange this 
module to make the system compatible for other platforms.