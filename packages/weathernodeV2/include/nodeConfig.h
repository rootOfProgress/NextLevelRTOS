#ifndef NODE_CONFIG_H
#define NODE_CONFIG_H

enum 
{
  PackageAcknowledge = 0,
  DateTimeUpdate
};

enum 
{
  Am2302RetryCount = 100,
};

typedef struct AckPackage
{
  char typeOfAckPackage;
  char ackPayload[8];
} AckPackage_t;

#endif