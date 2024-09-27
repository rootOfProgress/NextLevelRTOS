#ifndef GLOBALS_H
#define GLOBALS_H

void append_os_core_function(unsigned int (*function)());
extern unsigned int (*osCoreFunctions[6])();
#endif