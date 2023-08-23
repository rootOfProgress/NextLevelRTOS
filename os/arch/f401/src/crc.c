#include "crc.h"
#include "rcc.h"
#include "lang.h"
CrcRegisterMap_t *crc_regs;


void crc_activate(void)
{
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RccBaseAdress;

    WRITE_REGISTER(&rcc_regs->ahb1enr, READ_REGISTER(&rcc_regs->ahb1enr) | (1 << 12));    
    crc_regs = (CrcRegisterMap_t*) CrcBaseAdress;
}

void crc_reset(void)
{
    WRITE_REGISTER(&crc_regs->cr, 1);
}

unsigned int crc_read(void)
{
    return READ_REGISTER(&crc_regs->dr);
}

void crc_feed(unsigned int value)
{
    WRITE_REGISTER(&crc_regs->dr, value);
}

void crc_store_temp(char value)
{
    WRITE_REGISTER(&crc_regs->idr, (unsigned int) value);
}
