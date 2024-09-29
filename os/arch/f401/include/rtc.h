#ifndef RTC_H
#define RTC_H

#include "util/timeFormats.h"

typedef struct RTCRegisterMap
{
    unsigned int TR;       // 0x00 RTC_TR
    unsigned int DR;       // 0x04 RTC_DR
    unsigned int CR;       // 0x08 RTC_CR
    unsigned int ISR;      // 0x0C RTC_ISR
    unsigned int PRER;     // 0x10 RTC_PRER
    unsigned int WUTR;     // 0x14 RTC_WUTR
    unsigned int CALIBR;   // 0x18 RTC_CALIBR
    unsigned int ALRMAR;   // 0x1C RTC_ALRMAR
    unsigned int ALRMBR;   // 0x20 RTC_ALRMBR
    unsigned int WPR;      // 0x24 RTC_WPR
    unsigned int SSR;      // 0x28 RTC_SSR
    unsigned int TSTR;     // 0x30 RTC_TSTR
    unsigned int TSSSR;    // 0x38 RTC_TSSSR
    unsigned int CALR;     // 0x3C RTC_CALR
    unsigned int TAFCR;    // 0x40 RTC_TAFCR
    unsigned int ALRMASSR; // 0x44 RTC_ALRMASSR
    unsigned int ALRMBSSR; // 0x48 RTC_ALRMBSSR
    unsigned int BKP0R;    // 0x50 RTC_BKP0R
    unsigned int BKP1R;    // 0x54 RTC_BKP1R
    unsigned int BKP2R;    // 0x58 RTC_BKP2R
    unsigned int BKP3R;    // 0x5C RTC_BKP3R
    unsigned int BKP4R;    // 0x60 RTC_BKP4R
    unsigned int BKP5R;    // 0x64 RTC_BKP5R
    unsigned int BKP6R;    // 0x68 RTC_BKP6R
    unsigned int BKP7R;    // 0x6C RTC_BKP7R
    unsigned int BKP8R;    // 0x70 RTC_BKP8R
    unsigned int BKP9R;    // 0x74 RTC_BKP9R
    unsigned int BKP10R;   // 0x78 RTC_BKP10R
    unsigned int BKP11R;   // 0x7C RTC_BKP11R
    unsigned int BKP12R;   // 0x80 RTC_BKP12R
    unsigned int BKP13R;   // 0x84 RTC_BKP13R
    unsigned int BKP14R;   // 0x88 RTC_BKP14R
    unsigned int BKP15R;   // 0x8C RTC_BKP15R
    unsigned int BKP16R;   // 0x90 RTC_BKP16R
    unsigned int BKP17R;   // 0x94 RTC_BKP17R
    unsigned int BKP18R;   // 0x98 RTC_BKP18R
    unsigned int BKP19R;   // 0x9C RTC_BKP19R
} RTCRegisterMap_t;

enum 
{
  RTCBaseAdress = 0x40002800
};

enum 
{
  SU = 0, // Einerstelle der Sekunden.
  ST = 4, // Zehnerstelle der Sekunden.
  MNU = 8, // Einerstelle der Minuten.
  MNT = 12, // Zehnerstelle der Minuten.
  HU = 16, // Einerstelle der Stunden.
  HT = 20 //  Zehnerstelle der Stunden.
};

enum 
{
  DU = 0, // Einerstelle des Tages.
  DT = 4, // Zehnerstelle des Tages.
  MU = 8, // Einerstelle des Monats.
  MT = 12, // Zehnerstelle des Monats.
  WDU = 13, // Wochentag
  YU = 16, // Einerstelle der Jahres.
  YT = 20 //  Zehnerstelle des Jahres.
};

void RTC_init(void);
void RTC_activateClockSource(void);
void RTC_activateBackupDomain(void);
void RTC_activateDeviceClock(void);
void RTC_activateInitMode(void);
void RTC_configPreDiv(void);
void RTC_set24Format(void);
void RTC_deactivateInitMode(void);
void RTC_setTimeAndDate(TimeRepresentation_t* time, DateRepresentation_t* date);
void RTC_readCurrentTime(TimeRepresentation_t* time);
void RTC_readCurrentDate(DateRepresentation_t* date);
void RTC_readCurrentDateTimeRaw(DateTimeRepresentationRaw_t* dateTime);
void RTC_collectConvertedDateTime(void);
#endif