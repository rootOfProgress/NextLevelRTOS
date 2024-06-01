#ifndef TIMER_COMMON_H
#define TIMER_COMMON_H

#define CEN 0

typedef struct timerRegisterMap
{
  unsigned int cr1; // 0x00
  unsigned int cr2; // 0x00
  unsigned int smcr; // 0x00
  unsigned int dier; // 0x00
  unsigned int sr; // 0x10
  unsigned int egr; // 0x14
  unsigned int ccmr1; // 0x18
  unsigned int ccmr2; // 0x00
  unsigned int ccer; // 0x20
  unsigned int cnt; // 0x00
  unsigned int psc; // 0x28
  unsigned int arr; // 0x2c
  unsigned int reserverd; // 0x30
  unsigned int ccr1; // 0x
  unsigned int ccr2; // 0x
  unsigned int ccr3; // 0x
  unsigned int ccr4; // 0x40
  unsigned int reserverd1; // 0x44
  unsigned int dcr; // 0x
  unsigned int dmar; // 0x4c
  unsigned int tim2_or; // 0x
  unsigned int tim5_orccr4; // 0x40

} timerRegisterMap_t;

#endif