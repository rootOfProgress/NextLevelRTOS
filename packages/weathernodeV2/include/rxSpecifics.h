#ifndef RX_SPECIFICS_H
#define RX_SPECIFICS_H

#include "util/timeFormats.h"

typedef enum AckTypes
{
  Ack_Default = 0,
  Ack_ContainsDate,
  Ack_ContainsTime,
  Ack_RequestLifetime,
  Ack_RequestErrorCode,
  Ack_RequestErrorLog
} AckTypes_t;

typedef struct AckPayload
{
  char ackType;
  union rxPayloadSpecifics
  {
    struct 
    {
      TimeRepresentation_t timeRepresentation;
      char reserved[3];
    } time;
    struct 
    {
      DateRepresentation_t dateRepresentation;
      char reserved[4];
    } date;
  } specific;
} AckPayload_t;

#endif