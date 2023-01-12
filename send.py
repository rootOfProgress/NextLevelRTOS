import serial
import struct
import os
from dataclasses import dataclass

ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=2, xonxoff=False, rtscts=False, dsrdtr=False) #Tried with and without the last 3 parameters, and also at 1Mbps, same happens.
# ser = serial.Serial('/tmp/s9.out', 9600, timeout=2, xonxoff=False, rtscts=False, dsrdtr=False) #Tried with and without the last 3 parameters, and also at 1Mbps, same happens.

ser.flushInput()
ser.flushOutput()

result = []

@dataclass
class ExternalBinary:
    adress:

#sed -i "1 s/.*/STARTADRESS\ =\ 0x200040a8;/" link_external.ld   

def recompile_binary(final_adress):
    print("Enter package name:")
    package_name = input()

    # print("Compiling...")
    # cmd = "make -C " + " ./packages/" + package_name
    # os.system(cmd)

    cmd = "cd ./packages/" + package_name
    os.system(cmd)    
    os.system("pwd")

    # print("Change di")
    final_adress_adjusted = final_adress + 1
    print("Alter adress from " + hex(final_adress) + " to " + hex(final_adress_adjusted))

    cmd = "sed -i \"1 s/.*/STARTADRESS\ =\ " + hex(final_adress_adjusted) + ";/\"" + " ./packages/" + package_name + "/link_external.ld"
    os.system(cmd)

    print("Recompile programm...")
    cmd = "make -C " + " ./packages/" + package_name
    os.system(cmd)

    # print(file_path)
    

def process_result():
    bts = b''.join(result[0:8])
    match bts.decode("utf-8"):
        case "AAAABBBB":
            print("Memory response from device")
            start_adress_bin = b''.join(result[8:17])
            start_adress = int(start_adress_bin.decode("utf-8"))
            # print(hex(start_adress))
            recompile_binary(start_adress)
        case _:
            generic = b''.join(result[0:]).decode("utf-8")
            print(generic)

    del result[:]
    print(bytes_read)

bytes_read = 0

def create_task():


while True:
    bytesToRead = ser.inWaiting()
    incoming_byte = ser.read(bytesToRead)
    if bytesToRead != 0:
        result.append(incoming_byte)
        bytes_read += 1
        if bytes_read == 64:
            create_task()
            # process_result()
            bytes_read = 0