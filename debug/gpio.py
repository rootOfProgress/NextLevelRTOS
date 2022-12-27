import gdb
from enum import Enum
import unicodedata

GPIO_A = 0x48000000
MODER = 0x48000000
OTYPER = 0x48000004
SPEEDR = 0x48000008
PUPDR = 0x4800000C
IDR = 0x48000010
ODR = 0x48000014
BSRR = 0x48000018
LCKR = 0x4800001C
AFRL = 0x48000020
AFRH = 0x48000024
BRR = 0x48000028

class GpioRegs(Enum):
    GPIO_A = 0x48000000
    MODER = 0x48000000
    OTYPER = 0x48000004
    SPEEDR = 0x48000008
    PUPDR = 0x4800000C
    IDR = 0x48000010
    ODR = 0x48000014
    BSRR = 0x48000018
    LCKR = 0x4800001C
    AFRL = 0x48000020
    AFRH = 0x48000024
    BRR = 0x48000028    

regs = [
    "MODER",
    "OTYPER",
    "SPEEDR",
    "PUPDR",
    "IDR",
    "ODR",
    "BSRR",
    "LCKR",
    "AFRL",
    "AFRH",
    "BRR",    
]


def remove_control_characters(s):
    return "".join(ch for ch in s if unicodedata.category(ch)[0]!="C")

def get_high(reg_content) -> list:
    reg = int(remove_control_characters(reg_content), 16)
    bits_set = []
    for bit in range(0,32):
        if (reg & 1 << bit):
            bits_set.append(bit)
    return bits_set

def print_gpio_a_state():
    idx = 0
    for reg in range(GPIO_A, GPIO_A + 0x2C, 0x04):
        foo = ""
        command = "x " + "0x{0:02x}".format(reg)
        foo = gdb.execute(command, False, True)
        bits_on_high = get_high(foo.split(':')[1])
        print(command + " " + regs[idx] + " -> " + remove_control_characters(foo.split(':')[1]) + " Bits set: " + str(bits_on_high))
        idx += 1

print_gpio_a_state()