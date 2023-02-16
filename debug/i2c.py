import gdb
from enum import Enum
import unicodedata

I2C1 = 0x40005400

regs = [
    "CR1",
    "CR2",
    "OAR1",
    "OAR2",
    "TIMINGR",
    "TIMEOUTR",
    "ISR",
    "ICR",
    "PECR",
    "RXDR",
    "TXDR",    
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

def print_i2c_state():
    idx = 0
    for reg in range(I2C1, I2C1 + 0x2C, 0x04):
        foo = ""
        command = "x " + "0x{0:02x}".format(reg)
        foo = gdb.execute(command, False, True)
        bits_on_high = get_high(foo.split(':')[1])
        print(command + " " + regs[idx] + " -> " + remove_control_characters(foo.split(':')[1]) + " Bits set: " + str(bits_on_high))
        idx += 1

print_i2c_state()