#include "spi.h"
#include "gpio/gpio.h"
#include "rcc.h"
#include "lang.h"

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

void init_spi_gpio(GpioObject_t* obj)
{
  init_gpio(obj);

  // clk
  obj->pin = 3;
  obj->port = 'B';
  set_moder(obj, AlternateFunctionMode);
  into_af(obj, 5);
  set_otyper(obj, PushPull);
  set_pupdr(obj, PullDown);
  set_speed(obj, High);

  // miso
  obj->pin = 4;
  obj->port = 'B';
  set_otyper(obj, PushPull);
  set_pupdr(obj, PullUp);
  set_speed(obj, High);
  set_moder(obj, AlternateFunctionMode);
  into_af(obj, 5);

  // mosi
  obj->pin = 5;
  obj->port = 'B';
  set_otyper(obj, OpenDrain);
  set_pupdr(obj, PullUp);
  set_speed(obj, High);
  set_moder(obj, AlternateFunctionMode);
  into_af(obj, 5);

  GpioObject_t portA;
  portA.port = 'A';
  init_gpio(&portA);

  // csl
  portA.pin = 15;
  set_pupdr(&portA, PullUp);
  set_otyper(&portA, PushPull);
  set_moder(&portA, AlternateFunctionMode);
  into_af(&portA, 5);

  // // cen
  portA.pin = 5;
  set_pupdr(&portA, PullDown);
  set_moder(&portA, GeneralPurposeOutputMode);
  set_pin_off(&portA);
}

void init_spi(void)
{
  GpioObject_t t;
  t.port = 'B';
  init_spi_gpio(&t);

  SpiRegisterMap_t* spi_regs = (SpiRegisterMap_t*) Spi1BaseAdress;
  RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RccBaseAdress;

  WRITE_REGISTER(&rcc_regs->apb2enr, READ_REGISTER(&rcc_regs->apb2enr) | (1 << SPI1EN));
  // WRITE_REGISTER(&spi_regs->cr1, READ_REGISTER(&spi_regs->cr1) & ~(1 << SPE));
  WRITE_REGISTER(&spi_regs->cr1, 0);
  WRITE_REGISTER(&spi_regs->cr1, \
                 (7 << BR) \
                 | (1 << MSTR) \
                 | (1 << SSM) \
                 | (1 << SSI)\
                );

  WRITE_REGISTER(&spi_regs->cr2, 0);
  WRITE_REGISTER(&spi_regs->cr2, (1 << SSOE));
}

void NO_OPT spi_read(char *data, unsigned int length, char *receive_buffer)
{
  SpiRegisterMap_t* spi_regs = (SpiRegisterMap_t*) Spi1BaseAdress;
  WRITE_REGISTER(&spi_regs->cr1, READ_REGISTER(&spi_regs->cr1) | (1 << SPE));

  for (unsigned int t = 0; t <= length; t++)
  {
    WRITE_REGISTER(&spi_regs->dr, data[t]);
    while (!((READ_REGISTER(&(spi_regs)->sr) & (1 << BSY)) == 0));
    char spiRetour = READ_REGISTER(&spi_regs->dr);
    if (t != 0)
    {
      receive_buffer[t - 1] = spiRetour;
    }
  }

  WRITE_REGISTER(&spi_regs->cr1, READ_REGISTER(&spi_regs->cr1) & ~(1 << SPE));
}

void NO_OPT spi_write(char *data, unsigned int length)
{
  SpiRegisterMap_t* spi_regs = (SpiRegisterMap_t*) Spi1BaseAdress;

  WRITE_REGISTER(&spi_regs->cr1, READ_REGISTER(&spi_regs->cr1) | (1 << SPE));

  for (unsigned int t = 0; t <= length; t++)
  {
    WRITE_REGISTER(&spi_regs->dr, data[t]);
    while (!((READ_REGISTER(&(spi_regs)->sr) & (1 << BSY)) == 0));
  }

  WRITE_REGISTER(&spi_regs->cr1, READ_REGISTER(&spi_regs->cr1) & ~(1 << SPE));
}