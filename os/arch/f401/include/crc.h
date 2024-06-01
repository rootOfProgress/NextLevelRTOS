#ifndef CRC_H
#define CRC_H

enum { CrcBaseAdress = 0x40023000 };

typedef struct CrcRegisterMap
{
  unsigned int dr;
  unsigned int idr;
  unsigned int cr;
} CrcRegisterMap_t;

void crc_activate(void);
void crc_reset(void);
unsigned int crc_read(void);
void crc_feed(unsigned int);
void crc_store_temp(char);

#endif