import gdb
from enum import Enum
import unicodedata
import json

from dataclasses import dataclass

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def print_info(str):
    print(bcolors.OKCYAN + str + bcolors.ENDC)

def print_fail(str):
    print(bcolors.FAIL + str + bcolors.ENDC)

def print_success(str):
    print(bcolors.OKGREEN + str + bcolors.ENDC)

def print_warning(str):
    print(bcolors.WARNING + str + bcolors.ENDC)

@dataclass
class Queue_t:
    def __init__(self, size, head, tail):
        self.size = size  
        self.head = head  
        self.tail = tail  

# @dataclass
class Tcb_t:
    def __init__(self, pid, sp, memory_lower_bound, memory_upper_bound):
        self.pid = pid 
        self.sp = sp 
        self.memory_lower_bound = memory_lower_bound 
        self.memory_upper_bound = memory_upper_bound 
    def __str__(self):
        return f"pid: {bcolors.FAIL} {self.pid} {bcolors.ENDC}\n\
                 sp: {self.sp}\n\
                 memory_lower_bound: {self.memory_lower_bound}\n\
                 memory_upper_bound: {self.memory_upper_bound}"

class Node_t:
    def __init__(self, next, prev, data, local):
        self.local = local
        self.next = next  
        self.prev = prev  
        self.data = data  
    def __str__(self):
        return f"local: {self.local}\nnext: {self.next}\nprev: {self.prev}\ndata: {self.data}"

def remove_control_characters(s):
    return "".join(ch for ch in s if unicodedata.category(ch)[0]!="C")

def parse(payload, num_of_entries):
    results = []
    payload_as_list = list(payload.replace(',','').split('\n'))
    for i in range(1, num_of_entries + 1):
        results.append(payload_as_list[i].split(' ')[4])
    return list(results)

def walk_node(node_adress, t, max_depth):
    if (t == max_depth):
        return
    command = "p *((Node_t *) " + hex(node_adress) + ")"

    node = gdb.execute(command, False, True)
    res = parse(node,3)
    command = "p *((Tcb_t *) " + res[2] + ")"
    tcb_info = gdb.execute(command, False, True)

    tcb_result = parse(tcb_info, 4)
    tcb_t = Tcb_t(tcb_result[0],tcb_result[1],tcb_result[2],tcb_result[3])
    print(tcb_t)
    
    node_t = Node_t(res[0],res[1], res[2], hex(node_adress))
    print(node_t)
    print('')
    walk_node(int(node_t.next, 16), t + 1, max_depth)

def walk_list(list_name):
    args = get_list(list_name)
    head_address = args[1]
    walk_node(head_address, 0, args[0])

    
        

def get_list(list_name):
        command = "p *((Queue_t *) " + list_name + ")"
        running_tasks = gdb.execute(command, False, True)
        running_tasks_as_list = list(running_tasks.replace(',','').split('\n'))

        size = running_tasks_as_list[1].split(' ')[4]
        head = running_tasks_as_list[2].split(' ')[4]
        tail = running_tasks_as_list[3].split(' ')[4]
        return (int(size, 16),int(head, 16),int(tail, 16))

# print(get_list("waiting_tasks"))
# print(get_list("running_tasks"))
walk_list("running_tasks")