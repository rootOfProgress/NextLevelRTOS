import serial
import struct
import os
from dataclasses import dataclass
from threading import Thread
import binascii
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

# @dataclass
# class ExternalBinary:
#     adress:

#sed -i "1 s/.*/STARTADRESS\ =\ 0x200040a8;/" link_external.ld   

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
    # size_as_hex = int(size)
    # size_as_hex = hex(int(size)).strip()

    # print(len(size_as_hex))
    print(size.to_bytes(4,'big'))
    text_file = open("./packages/" + package_name + "/prepare", "wb")
    text_file.write(size.to_bytes(4,'big'))
    text_file.close()

    print("set device into receiving state...")
    cmd = "printf \"\\x01\\x12\\x34\\x56\" >> /dev/ttyUSB0"
    os.system(cmd)

    receiver = Thread(target = device_rx)
    receiver.start()
    print("send binary size to device...")
    cmd = "dd if=./packages/" + package_name + "/prepare of=/dev/ttyUSB0 iflag=direct,skip_bytes"
    os.system(cmd)

    bytes_read = 0
    print("waiting for device response...")
    receiver.join()
    start_adress = process_result()
    recompile_binary(start_adress, package_name, size)
    # while True:
    #     bytesToRead = ser.inWaiting()
    #     incoming_byte = ser.read(bytesToRead)
    #     if bytesToRead != 0:
    #         result.append(incoming_byte)
    #         bytes_read += 1
    #         if bytes_read == 64:
    #             # create_task()
    #             process_result()
    #             bytes_read = 0

    # size_as_hex = '0x' + hex(int(size)).zfill(7)
    # print(size_as_hex)

def recompile_binary(final_adress, package_name, size):
    # print("Enter package name:")
    # package_name = input()

    # print("Compiling...")
    # cmd = "make -C " + " ./packages/" + package_name
    # os.system(cmd)

    cmd = "cd ./packages/" + package_name
    os.system(cmd)    
    os.system("pwd")

    # print("Change di")
    final_adress_adjusted = final_adress + 1
    print("Alter adress from " + hex(final_adress) + " to " + hex(final_adress_adjusted))

    # change startadress
    print_info("change startadress in linker script...")
    cmd = "sed -i \"1 s/.*/STARTADRESS\ =\ " + hex(final_adress_adjusted) + ";/\"" + " ./packages/" + package_name + "/link_external.ld"
    os.system(cmd)

    # change length
    print_info("change section size in linker script...")
    cmd = "sed -i \"2 s/.*/SIZE\ =\ " + str(size) + ";/\"" + " ./packages/" + package_name + "/link_external.ld"
    os.system(cmd)

    print("Recompile programm...")
    cmd = "make -C " + " ./packages/" + package_name
    os.system(cmd)


    # print(file_path)
    

def process_result():
    bts = b''.join(result[0:8])
    match bts.decode("utf-8"):
        case "AAAABBBB":
            start_adress_bin = b''.join(result[8:17])
            start_adress = int(start_adress_bin.decode("utf-8"))
            print_success("Got memory response from device, Startadress is : " + hex(start_adress))
            del result[:]
            return start_adress
            # recompile_binary(start_adress)
        case _:
            generic = b''.join(result[0:]).decode("utf-8")
            print(generic)

    del result[:]


# def create_task():


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

if __name__ == '__main__':
    receiver = Thread(target = device_rx)
    # receiver.start()
    while True:
        print("Select function")
        print("1: Upload Binary")
        print("2: Fetch OS Lifetime info")
        n = input()
        match int(n):
            case 1:
                print("Enter package name:")
                package_name = input()
                upload_binary("legacy_drivers")
                # upload_binary("ext")
    receiver.join()