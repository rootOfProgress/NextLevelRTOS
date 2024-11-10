#ifndef NODE_CONFIG_H
#define NODE_CONFIG_H

typedef enum NodeState
{
  NodeOperation_ProcessWeatherdata = 0,
  NodeOperation_SetTime,
  NodeOperation_SetDate,
  NodeOperation_Reboot,
  NodeOperation_RequestLifetime,
  NodeOperation_RequestErrorCode,
  NodeOperation_RequestErrorLog,
} NodeState_t;

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