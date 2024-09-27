#ifndef SOFT_CRC_H
#define SOFT_CRC_H

/**
 * @brief Calculate CRC-32 for a single byte, updating the previous CRC value.
 *
 * This function updates the CRC-32 checksum for a given byte, considering a previously
 * calculated CRC value. It is typically used iteratively over a stream of data.
 *
 * @param previous_crc The CRC value from the previous calculation step.
 *                     Use 0xFFFFFFFF for the initial call.
 * @param data         The input byte to be included in the CRC calculation.
 *
 * @return Updated CRC-32 value.
 */
unsigned int soft_crc32(unsigned int previous_crc, char data);

#endif