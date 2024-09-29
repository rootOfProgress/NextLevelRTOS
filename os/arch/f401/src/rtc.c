#include "rtc.h"
#include "rcc.h"
#include "lang.h"
#include "pwr.h"

void RTC_activateClockSource(void)
{
  RccRegisterMap_t* rcc_registers = (RccRegisterMap_t*) RccBaseAdress;

  enum 
  {
    LSEON = 0,
    LSERDY = 1,
    RTCSEL = 8
  };

  enum 
  {
    RTCSEL_LSE = 1 << RTCSEL
  };

  WRITE_REGISTER(&rcc_registers->bdcr, 
                 READ_REGISTER(&rcc_registers->bdcr) | (1 << LSEON)
                 );

  while (!(READ_REGISTER(&rcc_registers->bdcr) & (1 << LSERDY))) {};

  WRITE_REGISTER(&rcc_registers->bdcr, 
                 READ_REGISTER(&rcc_registers->bdcr) | RTCSEL_LSE
                 );
}

void RTC_activateBackupDomain(void)
{
  RccRegisterMap_t* rcc_registers = (RccRegisterMap_t*) RccBaseAdress;
  PWRRegisterMap_t* pwr_registers = (PWRRegisterMap_t*) PWRBaseAdress;

  enum
  {
    PWREN = 28
  };

  WRITE_REGISTER(&rcc_registers->apb1enr, 
                 READ_REGISTER(&rcc_registers->apb1enr) | (1 << PWREN)
                 );

  enum
  {
    DBP = 8
  };

  WRITE_REGISTER(&pwr_registers->CR, 
                 READ_REGISTER(&pwr_registers->CR) | (1 << DBP)
                 );
}

void RTC_activateDeviceClock(void)
{
  RccRegisterMap_t* rcc_registers = (RccRegisterMap_t*) RccBaseAdress;

  enum
  {
    RTCEN = 15
  };

  WRITE_REGISTER(&rcc_registers->bdcr, 
                 READ_REGISTER(&rcc_registers->bdcr) | (1 << RTCEN)
                 );
}

void RTC_activateInitMode(void)
{
  RTCRegisterMap_t* rtc_registers = (RTCRegisterMap_t*) RTCBaseAdress;

  enum
  {
    INITF = 6,
    INIT = 7
  };

  // unlock WPR
  WRITE_REGISTER(&rtc_registers->WPR, 0xCA);  
  WRITE_REGISTER(&rtc_registers->WPR, 0x53);  

  WRITE_REGISTER(&rtc_registers->ISR, 
                 READ_REGISTER(&rtc_registers->ISR) | (1 << INIT)
                 );

  while (!(READ_REGISTER(&rtc_registers->ISR) & (1 << INITF))) {};
}

void RTC_configPreDiv(void)
{
  RTCRegisterMap_t* rtc_registers = (RTCRegisterMap_t*) RTCBaseAdress;
  // PREDIV_A = 127, PREDIV_S = 255
  // 1 HZ Resolution
  WRITE_REGISTER(&rtc_registers->PRER, (127 << 16) | 255);
}

void RTC_set24Format(void)
{
  RTCRegisterMap_t* rtc_registers = (RTCRegisterMap_t*) RTCBaseAdress;
  
  enum
  {
    FMT = 6
  };

  WRITE_REGISTER(&rtc_registers->CR, READ_REGISTER(&rtc_registers->CR) & ~(1 << FMT)); 
}

void RTC_deactivateInitMode(void)
{
  RTCRegisterMap_t* rtc_registers = (RTCRegisterMap_t*) RTCBaseAdress;

  enum
  {
    RSF = 5,
    INITF = 6,
    INIT = 7
  };

  WRITE_REGISTER(&rtc_registers->ISR, 
                 READ_REGISTER(&rtc_registers->ISR) & ~(1 << INIT)
                 );

  while (!(READ_REGISTER(&rtc_registers->ISR) & (1 << RSF))) {};
}

void RTC_setTimeAndDate(TimeRepresentation_t* time, DateRepresentation_t* date)
{
  RTCRegisterMap_t* rtc_registers = (RTCRegisterMap_t*) RTCBaseAdress;
  RTC_activateInitMode();

  if (time)
  {
    WRITE_REGISTER(&rtc_registers->TR, 
                  ((time->hour / 10) << HT) | ((time->hour % 10) << HU) |
                  ((time->minute / 10) << MNT) | ((time->minute % 10) << MNU) |
                  ((time->second / 10) << ST) | ((time->second % 10) << SU));
  }

  if (date)
  {
    WRITE_REGISTER(&rtc_registers->DR, 
                  ((date->year / 10) << YT) | ((date->year % 10) << YU) |
                  ((unsigned int) date->weekDay << WDU) |
                  ((date->month / 10) << MT) | ((date->month % 10) << MU) |
                  ((date->day / 10) << DT) | ((date->day % 10) << DU));
  }
  RTC_deactivateInitMode();
}

void RTC_readCurrentTime(TimeRepresentation_t* time)
{
  RTCRegisterMap_t* rtc_registers = (RTCRegisterMap_t*) RTCBaseAdress;

  unsigned int timeRaw = READ_REGISTER(&rtc_registers->TR);
  time->hour = (((timeRaw >> HT) & 0x3) * 10) + ((timeRaw >> HU) & 0xF);
  time->minute = (((timeRaw >> MNT) & 0x7) * 10) + ((timeRaw >> MNU) & 0xF);
  time->second = (((timeRaw >> ST) & 0x7) * 10) + (timeRaw & 0xF);
}

void RTC_readCurrentDate(DateRepresentation_t* date)
{
  RTCRegisterMap_t* rtc_registers = (RTCRegisterMap_t*) RTCBaseAdress;

  unsigned int dateRaw = READ_REGISTER(&rtc_registers->DR);
  date->year = (((dateRaw >> YT) & 0xF) * 10) + ((dateRaw >> YU) & 0xF);
  date->weekDay = (WeekDay_t) ((dateRaw >> WDU) & 0x7);
  date->month = (((dateRaw >> MT) & 0x1) * 10) + ((dateRaw >> MU) & 0xF);
  date->day = (((dateRaw >> DT) & 0x3) * 10) + (dateRaw & 0xF);
}

void RTC_readCurrentDateTimeRaw(DateTimeRepresentationRaw_t* dateTime)
{
  RTCRegisterMap_t* rtc_registers = (RTCRegisterMap_t*) RTCBaseAdress;
  dateTime->time = READ_REGISTER(&rtc_registers->TR);
  dateTime->date = READ_REGISTER(&rtc_registers->DR);
}

// void RTC_collectConvertedDateTime(char* buffer)
// {
//   // memcpy_byte not available here
//   // TimeRepresentation_t currentTime;
//   // DateRepresentation_t currentDate;
  
//   // RTC_readCurrentTime(&currentTime);
//   // RTC_readCurrentDate(&currentDate);

//   // char *helper = buffer;

//   // memcpy_byte(helper, &currentTime, sizeof(TimeRepresentation_t));
//   // helper += sizeof(TimeRepresentation_t);
//   // memcpy_byte(helper, &currentDate, sizeof(DateRepresentation_t));
// }


void RTC_init(void)
{
  RTC_activateBackupDomain();
  RTC_activateClockSource();
  RTC_activateDeviceClock();

  RTC_activateInitMode();
  RTC_configPreDiv();
  RTC_set24Format();
  RTC_deactivateInitMode();
}
