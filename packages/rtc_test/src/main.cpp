#include "rtc.h"
#include <cstddef>
#include "os_api.h"

extern "C" void *memcpy(void* dest, const void* src,
                    size_t n)
{
  memcpy_byte(dest, src, n);
  return dest;
}

extern "C" void abort(void)
{
  while (1);
}

void* operator new(size_t size) 
{
  return allocate(size);
}

void operator delete(void* ptr) noexcept 
{
  deallocate((unsigned int*) ptr);
}

void operator delete(void* ptr, size_t size) noexcept 
{
  deallocate((unsigned int*) ptr);
}

class RTCTest 
{       
  private:             
    TimeRepresentation_t currenTime;
    DateRepresentation_t currenDate;

};

int main(void)
{

  RTCTest *rtc = new RTCTest();

  
  
  delete rtc;
  return 0;
}