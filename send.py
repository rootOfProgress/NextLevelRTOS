import serial
import struct
import os
import time
from dataclasses import dataclass
from threading import Thread
import binascii
from struct import pack, unpack
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=2, xonxoff=False, rtscts=False, dsrdtr=False) #Tried with and without the last 3 parameters, and also at 1Mbps, same happens.
# ser = serial.Serial('/tmp/s9.out', 9600, timeout=2, xonxoff=False, rtscts=False, dsrdtr=False) #Tried with and without the last 3 parameters, and also at 1Mbps, same happens.

ser.flushInput()
ser.flushOutput()

result = []

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


def upload_binary(package_name):
    cmd = "cd ./packages/" + package_name
    os.system(cmd)    
    os.system("pwd")

    print_info("Compile programm...")
    cmd = "make -C " + " ./packages/" + package_name
    os.system(cmd)

    print_info("Get Size...")
    cmd = "wc -c < " + " ./packages/" + package_name + "/build/" + package_name + "_binary"
    size = int(os.popen(cmd).read())

    if size > 0:
        print_success("Success - Filesize is : " + hex(size) + " / " + str(size))
    else:
        print_fail("Could not read filesize!")

    text_file = open("./packages/" + package_name + "/prepare", "wb")
    text_file.write(size.to_bytes(4,'big'))
    text_file.close()

    print_info("set device into receiving state...")
    cmd = "printf \"\\x01\\x12\\x34\\x56\" >> /dev/ttyUSB0"
    os.system(cmd)

    time.sleep(1)
    receiver = Thread(target = device_rx)
    receiver.start()

    print_info("send binary size to device...")
    cmd = "dd if=./packages/" + package_name + "/prepare of=/dev/ttyUSB0 iflag=direct,skip_bytes"
    os.system(cmd)
    time.sleep(1)

    bytes_read = 0
    print_info("waiting for device response...")
    start = time.time()
    
    receiver.join()

    stop = time.time()
    print_info("Took " + str(stop - start) + " ms")

    start_adress = process_result()
    recompile_binary(start_adress, package_name, size)
    time.sleep(1)

    print_info("upload binary to target...")
    cmd = "dd if=./packages/" + package_name + "/build/" + package_name + "_binary" + " of=/dev/ttyUSB0 iflag=direct,skip_bytes"
    os.system(cmd)   

    print_success("done!")


def recompile_binary(final_adress, package_name, size):
    cmd = "cd ./packages/" + package_name
    os.system(cmd)    
    os.system("pwd")

    final_adress_adjusted = final_adress - 1
    print_info("Alter adress from " + hex(final_adress) + " to " + hex(final_adress_adjusted))

    # change startadress
    print_info("change startadress in linker script...")
    cmd = "sed -i \"1 s/.*/STARTADRESS\ =\ " + hex(final_adress_adjusted) + ";/\"" + " ./packages/" + package_name + "/link_external.ld"
    os.system(cmd)

    print_info("recompile programm...")
    cmd = "make -C " + " ./packages/" + package_name
    os.system(cmd)

def process_result():
    # byte 0..7 : code
    bts = b''.join(result[0:8])
    match bts.decode("utf-8"):
        case "AAAABBBB": #memadress
            start_adress_bin = b''.join(result[8:17])
            start_adress = int(start_adress_bin.decode("utf-8"))
            print_success("Got memory response from device, Startadress is : " + hex(start_adress))
            del result[:]
            return start_adress
            # recompile_binary(start_adress)
        case "BBBBAAAA": #statistic
            # format
            # typedef struct MemoryStatistic {
            #     unsigned int num_of_allocs; byte 8..15 :  
            #     unsigned int num_of_deallocs; byte 16..23 : 
            #     unsigned int num_of_fractial_allocs; byte 24..31 : 
            #     unsigned int total_byte_alloced; byte 32..39 : 
            #     unsigned int total_byte_used; byte 40..47 : 
            #     unsigned int os_data_end; byte 48..55 : 
            #     unsigned int free_useable; byte 56..63 : 
            #     unsigned int waiting_tasks; byte 64..71 : 
            #     unsigned int total_scheduled_tasks; byte 72..79 : 
            #     unsigned int cpu_load; byte 80..87 : 
            # } MemoryStatistic_t;
            print_success("Got device response!")
            response = {
                "num_of_allocs" : unpack('I', b''.join(result[8:12]))[0],
                "num_of_deallocs" : unpack('I', b''.join(result[12:16]))[0], 
                "num_of_fractial_allocs" : unpack('I', b''.join(result[16:20]))[0], 
                "total_byte_alloced" : unpack('I', b''.join(result[20:24]))[0], 
                "total_byte_used" : unpack('I', b''.join(result[24:28]))[0], 
                "os_data_end" : unpack('I', b''.join(result[28:32]))[0], 
                "free_useable" : unpack('I', b''.join(result[32:36]))[0], 
                "waiting_tasks" : unpack('I', b''.join(result[36:40]))[0], 
                "total_scheduled_tasks" : unpack('I', b''.join(result[40:44]))[0], 
                "cpu_load" : unpack('I', b''.join(result[44:48]))[0],                
            }
            print(response)
            # num_of_allocs = unpack('I', b''.join(result[8:12]))[0]
            # num_of_deallocs = unpack('I', b''.join(result[12:16]))[0] 
            # num_of_fractial_allocs = unpack('I', b''.join(result[16:20]))[0] 
            # total_byte_alloced = unpack('I', b''.join(result[20:24]))[0] 
            # total_byte_used = unpack('I', b''.join(result[24:28]))[0] 
            # os_data_end = unpack('I', b''.join(result[28:32]))[0] 
            # free_useable = unpack('I', b''.join(result[32:36]))[0] 
            # waiting_tasks = unpack('I', b''.join(result[36:40]))[0] 
            # total_scheduled_tasks = unpack('I', b''.join(result[40:44]))[0] 
            # cpu_load = unpack('I', b''.join(result[44:48]))[0] 
            # print("num_of_allocs: ", num_of_allocs)
            # print("num_of_deallocs: ", num_of_deallocs)
            # print("num_of_fractial_allocs: ", num_of_fractial_allocs)
            # print("total_byte_alloced: ", total_byte_alloced)
            # print("total_byte_used: ", total_byte_used)
            # print("os_data_end: ", os_data_end)
            # print("free_useable: ", free_useable)
            # print("waiting_tasks: ", waiting_tasks)
            # print("total_scheduled_tasks: ", total_scheduled_tasks)
            # print("cpu_load: ", cpu_load)
            
            del result[:]
            return
        case _:
            print_fail("Unknown response!")
            generic = b''.join(result[0:]).decode("utf-8")
            print(generic)


# def create_task():
def fetch_lifetime_info():
    print_info("set device into REQUEST_STATISTIC state...")
    cmd = "printf \"\\x03\\x12\\x34\\x56\" >> /dev/ttyUSB0"
    os.system(cmd)
    time.sleep(1)

    receiver = Thread(target = device_rx)
    receiver.start()

    print_info("trigger update...")
    cmd = "printf \"\\x03\" >> /dev/ttyUSB0"
    os.system(cmd)

    print_info("waiting for device response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    print_info("Took " + str(stop - start) + " ms")
    process_result()


def device_rx():
    bytes_read = 0
    while True:
        bytesToRead = ser.inWaiting()
        incoming_byte = ser.read(bytesToRead)
        if bytesToRead != 0:
            result.append(incoming_byte)
            bytes_read += 1
            if bytes_read == 64:
                return
                # create_task()
                # process_result()
                # bytes_read = 0


def alter_speed(speed):
    print_info("set device into speed state...")
    cmd = "printf \"\\x04\\x12\\x34\\x56\" >> /dev/ttyUSB0"
    os.system(cmd) 
    time.sleep(1)

    n = list(speed)
    cmd_0 = "printf \"" 
    cmd_1 = "" 
    
    for l in n:
        cmd_1 += "\\\\x" + f'{ord(l):x}' 

    line = cmd_0 + cmd_1 + "\" >> /dev/ttyUSB0"
    os.system(line)
    # print(line)

if __name__ == '__main__':
    receiver = Thread(target = device_rx)
    # receiver.start()
    while True:
        print("Select function")
        print("1: Upload Binary")
        print("2: Fetch OS Lifetime info")
        print("3: Alter Engine Speed")
        n = input()
        # n = 3
        match int(n):
            case 1:
                print("Enter package name:")
                package_name = input()
                upload_binary(package_name)
            case 2:
                fetch_lifetime_info()
            case 3:
                print("EngineSpeed:")
                n = input()
                alter_speed(n)

    receiver.join()