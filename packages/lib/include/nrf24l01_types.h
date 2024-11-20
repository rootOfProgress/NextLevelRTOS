#ifndef NRF24L01_TYPES_H
#define NRF24L01_TYPES_H

#include <stdint.h>

enum 
{
  MaxPayloadWeight = 31,
  MaxPayloadSizeWithoutCRC = MaxPayloadWeight - sizeof(unsigned int),
  MaxUseablePayloadSize = MaxPayloadSizeWithoutCRC - sizeof(unsigned int)
};


typedef struct PackageFrame
{
  unsigned int crc;
  char indexOfThisPackage;
  char totalAmountOfPackages;
  char containsTransmitterState;
  char reserved;
  char payload[MaxUseablePayloadSize];
} PackageFrame_t;

typedef struct TransmitterState
{
  char totalLostPackages;
  char signalStrength;
  char retransmitsForThisPackage : 4, currentChannel : 4;
  char reserved;
} TransmitterState_t;

#endif