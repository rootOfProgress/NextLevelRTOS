#ifndef NRF_DRIVER_H
#define NRF_DRIVER_H

void nrf_power_off(void);
void nrf_power_on(void);
char configure_device(Nrf24l01Registers_t*, OperatingMode_t);
void get_nrf_config(Nrf24l01Registers_t*);
void nrf_transmit(char*, unsigned int);


#endif