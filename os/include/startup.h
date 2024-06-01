#ifndef STARTUP_H
#define STARTUP_H

typedef enum RebootTypes
{
  RebootRequestedByOperator = 1 << 0,
  RebootAfterHardFault = 1 << 1,
  RebootAfterUsageFault = 1 << 2,
  RebootAfterKernelError = 1 << 3,
  IsColdStart = 1 << 4,
  RebootRequestedByOperatorKeepStat = 1 << 0,
} RebootTypes_t;

typedef struct BootFlags
{
  char isColdStart : 1, : 7;
  RebootTypes_t reboot_type;
} BootFlags_t;

extern BootFlags_t boot_flags;

#endif