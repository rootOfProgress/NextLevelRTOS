#ifndef EXCEPTION_INLINES_H
#define EXCEPTION_INLINES_H

static inline __attribute__((always_inline)) void save_psp_if_threadmode(void)
{
  __asm volatile (
    "TST lr, #4\n"
    "ITTT NE\n"
    "MRSNE r2, PSP\n"
    "STMDBNE r2!, {r4-r11}\n"
    "MSRNE PSP, r2\n"
  );
}

static inline __attribute__((always_inline)) void restore_psp()
{
  __asm volatile (
    "mrs r2, psp\n"
    "ldmfd r2!, {r4-r11}\n"
    "msr psp, r2\n"
  );
}

/**
 * 
 * @param: None
 * @return: None
 * 
 * This function restores the not-automatically saved registers (i.e. r4 - r11) from the stack,
 * which adress is provided in register r2. First, the link register is checked if bit 2 LSB is set, that means:
 * 
 * * ThreadMode and MSP: 0xFFFFFFF9 & 0x4 -> 0   
 * * ThreadMode and PSP: 0xFFFFFFFD & 0x4 ->    
 * 
 */
static inline __attribute__((always_inline)) void restore_psp_if_threadmode()
{
  __asm volatile (
    "TST lr, #4\n"
    "ITTT NE\n"
    "MRSNE r2, psp\n"
    "LDMFDNE r2!, {r4-r11}\n"
    "MSRNE psp, r2\n"
  );
}

#endif