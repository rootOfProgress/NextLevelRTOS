#ifndef UART_H
#define UART_H

#include <stdint.h>

#define READ_REGISTER(addr)     (*(volatile uint32_t *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile uint32_t *) (addr)) = (uint32_t) (val))
#define WRITE_REGISTER64(addr, val) ((*(volatile uint64_t *) (addr)) = (uint64_t) (val))

#define USART1_SR 0x40011000
#define USART1_DR 0x40011004
#define USART1_CR 0x4001100C
#define UE 13
#define RXNEIE 5
#define RE 2
#define TE 3
#define BUFFERSIZE 100

extern uint32_t buffer[];

uint32_t pow_base_16(uint32_t);
uint32_t hex2int(char);

void send_number(uint32_t);
void init_uart(void);
void print(char *);
void __attribute__((interrupt)) uart_isr_handler(void);

#endif
