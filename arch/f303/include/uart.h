#ifndef UART_H
#define UART_H

// #include <stdint.h>

// #define READ_REGISTER(addr)     (*(volatile uint32_t *) (addr))
// #define WRITE_REGISTER(addr, val) ((*(volatile uint32_t *) (addr)) = (uint32_t) (val))
// #define WRITE_REGISTER64(addr, val) ((*(volatile uint64_t *) (addr)) = (uint64_t) (val))

const unsigned int USART1_BASEADRESS = 0x40013800;

extern unsigned int UE;
extern unsigned int RE;
extern unsigned int TE;
extern unsigned int RXNEIE;

typedef struct UartRegisterMap {
    unsigned int* cr1;
    unsigned int* cr2;
    unsigned int* cr3;
    unsigned int* brr;
    unsigned int* gtpr;
    unsigned int* rtor;
    unsigned int* rqr;
    unsigned int* isr;
    unsigned int* icr;
    unsigned int* rdr;
    unsigned int* tdr;
} UartRegisterMap_t;

void init_uart(void);
void print(char *);
unsigned int __attribute__ ((always_inline)) read_data_register(void);


#endif