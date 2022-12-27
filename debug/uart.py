import gdb
from enum import Enum
import unicodedata

USART1 = 0x40013800

regs = [
    "CR1",
    "CR2",
    "CR3",
    "BRR",
    "GTPR",
    "RTOR",
    "RQR",
    "ISR",
    "ICR",
    "RDR",
    "TDR",    
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
    for reg in range(USART1,USART1 + 0x2C, 0x04):
        foo = ""
        command = "x " + "0x{0:02x}".format(reg)
        foo = gdb.execute(command, False, True)
        bits_on_high = get_high(foo.split(':')[1])
        print(command + " " + regs[idx] + " -> " + remove_control_characters(foo.split(':')[1]) + " Bits set: " + str(bits_on_high))
        idx += 1

print_gpio_a_state()