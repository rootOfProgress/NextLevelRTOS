#ifndef TIME_FORMATS_H
#define TIME_FORMATS_H

typedef enum WeekDay
{
  MONDAY = 1,
  TUESDAY = 2,
  WEDNESDAY = 3,
  THURSDAY = 4,
  FRIDAY = 5,
  SATURDAY = 6,
  SUNDAY = 7,
} WeekDay_t;

typedef struct TimeRepresentation
{
  char hour;
  char minute;
  char second;
  char reserved;
} TimeRepresentation_t;

typedef struct DateRepresentation
{
  char year;
  char month;
  char day;
  WeekDay_t weekDay;
} DateRepresentation_t;

typedef struct DateTimeRepresentation
{
  TimeRepresentation_t time;
  DateRepresentation_t date;
} DateTimeRepresentation_t;

typedef struct DateTimeRepresentationRaw
{
  unsigned int time;
  unsigned int date;
} DateTimeRepresentationRaw_t;

#endif