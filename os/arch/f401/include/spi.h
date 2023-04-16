#ifndef SPI_H
#define SPI_H

enum { Spi1BaseAdress = 0x40013000 };

#define CPHA 0
#define CPOL 1
#define SPE 6
#define MSTR 2
#define SSI 8
#define BSY 7
#define SSM 9
#define TXE 1
#define SSOE 2
#define DFF 11
#define LSBFIRST 7
#define RXNE 0

typedef struct
{
    unsigned int cr1;
    unsigned int cr2;
    unsigned int sr;
    unsigned int dr;
    unsigned int crcpr;
    unsigned int rxcrcr;
    unsigned int txcrcr;
    unsigned int i2scfgr;
    unsigned int i2spr;
} SpiRegisterMap_t;

void init_spi(void);
void spi_write_single(char);
void spi_write(char preamble, char *data, unsigned int length, char *receive_buffer);

extern SpiRegisterMap_t* spi_regs;
#endif