#ifndef TEST_H
#define TEST_H

#include "nrf24l01.h"

char set_and_test_txaddr(void);
char get_and_test_nrf_register(Nrf24l01RegisterNames_t, char);
void self_check(void);

#endif