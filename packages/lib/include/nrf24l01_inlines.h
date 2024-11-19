#ifndef NRF24L01_INLINES_H
#define NRF24L01_INLINES_H

#include "nrf24l01_types.h"
#include "crc.h"
#include "soft_crc.h"

// @does not work yet!
// unsigned int calculateCrc(char *payload, char endpointIsRaspberryPi)
// {
//   unsigned int crc = 0xFFFFFFFF;
//   if (endpointIsRaspberryPi)
//   {
//     for (unsigned int i = 0; i < MaxPayloadSizeWithoutCRC; i++)
//     {
//         crc = soft_crc32(crc, payload[i]);
//     }
//   } 
//   else
//   {
//     crc_reset();
//     for (unsigned int i = 0; i < MaxPayloadSizeWithoutCRC; i++)
//     {
//       crc_feed((unsigned int) payload[i]);
//     }
//     crc = crc_read();
//   }

//   return crc;
// }

#endif