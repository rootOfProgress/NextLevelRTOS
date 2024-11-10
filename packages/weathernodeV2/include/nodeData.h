#ifndef NODE_DATA_H
#define NODE_DATA_H

#include "am2302.h"
#include "util/timeFormats.h"

typedef enum PackageType
{
  PackageType_WeatherData = 0,
  PackageType_DateTimeRequest,
} PackageType_t; 

typedef struct
{
  unsigned short version;
  unsigned short size;
} StructHeader_t;

typedef struct 
{
  char nodeNumber;
  char packageType;
  char reserved[2];
} PayloadMetaData_t;

typedef struct 
{
  char totalLostPackages;
  char retransmitsForThisPackage;
  char signalStrength;
  char reserved;
  unsigned short batteryHealth;
  char currentChannel;
  char reserved1[3];
} DeviceEnvironmentData_t;

typedef struct NodeFrame 
{
  StructHeader_t header;
  PayloadMetaData_t meta;
  union {
    DeviceEnvironmentData_t environment;
    DateTimeRepresentation_t dateTime;
  };
  Am2302Readings_t readings;
} NodePackage_t;

#endif