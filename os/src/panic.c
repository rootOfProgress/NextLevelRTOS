#include "panic.h"


void invoke_panic(PanicTypes_t panic_type)
{
    switch (panic_type)
    {
    case OUT_OF_MEMORY:
        while (1)
        {
            /* code */
        }
        
        break;
    case SCHEDULER_NOT_INITIALIZED:
        while (1)
        {
            /* code */
        }
        
    default:
        break;
    }
}