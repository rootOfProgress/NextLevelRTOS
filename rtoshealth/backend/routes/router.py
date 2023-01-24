from __main__ import app, path_suffix, package_path

import json
import os
import time
import serial

from threading import Thread, Lock
from struct import pack, unpack
from flask import request
from random import randrange
import datetime, timedelta
serial_device = None
result = []
device_address = ""
mutex = Lock()
is_busy = False

current_speed = {
      "engine_0": 0,
      "engine_1": 0,
      "engine_2": 0,
      "engine_3": 0,    
}


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

def flush_rx_buffer():
    serial_device.flush()
    serial_device.reset_input_buffer()
    serial_device.reset_output_buffer()

# def device_rx():
#     global result
#     global mutex
#     mutex.acquire()
#     bytes_read = 0
#     while True:
#         bytesToRead = serial_device.inWaiting()
#         incoming_byte = serial_device.read(bytesToRead)
#         if bytesToRead != 0:
#             result.append(incoming_byte)
#             bytes_read += 1
#             if bytes_read == 64:
#                 mutex.release()
#                 return

def device_rx():
    global result
    global mutex
    global is_busy
    mutex.acquire()
    is_busy = True
    bytes_read = 0
    while True:
        try:
            bytesToRead = serial_device.inWaiting()
            incoming_byte = serial_device.read(bytesToRead)
            # print("foo")
            if bytesToRead != 0:
                result.append(incoming_byte)
                bytes_read += 1
                if bytes_read == 64:
                    is_busy = False
                    print_success("All set!")
                    mutex.release()
                    return
        except:
            pass
            is_busy = False
            print_fail("Timeout!")
            mutex.release()
            return

    # while True:
    #     try:
    #         bytesToRead = serial_device.inWaiting()
    #         incoming_byte = serial_device.read(bytesToRead)
    #         if bytesToRead != 0:
    #             result.append(incoming_byte)
    #             bytes_read += 1
    #             if bytes_read == 64:
    #                 return
    #     except:
    #         print_fail("Overrun error!")
    #         flush_rx_buffer()

def flush():
    del result[:]

def process_result():
    # byte 0..7 : code
    bts = b''.join(result[0:8])
    try:
        match bts.decode("utf-8"):
            case "AAAABBBB": #memadress
                start_adress_bin = b''.join(result[8:17])
                start_adress = int(start_adress_bin.decode("utf-8"))
                print_success("Got memory response from device, Startadress is : " + hex(start_adress))
                del result[:]
                return start_adress
            case "BBBBAAAA":
                print_success("Got device response!")
                response = {
                    "num_of_allocs" : unpack('I', b''.join(result[8:12]))[0],
                    "num_of_deallocs" : unpack('I', b''.join(result[12:16]))[0], 
                    "ram_size" : unpack('I', b''.join(result[16:20]))[0], 
                    "total_byte_alloced" : unpack('I', b''.join(result[20:24]))[0], 
                    "total_byte_used" : unpack('I', b''.join(result[24:28]))[0], 
                    "os_data_end" : unpack('I', b''.join(result[28:32]))[0], 
                    "free_useable" : unpack('I', b''.join(result[32:36]))[0], 
                    "waiting_tasks" : unpack('I', b''.join(result[36:40]))[0], 
                    "total_scheduled_tasks" : unpack('I', b''.join(result[40:44]))[0], 
                    "cpu_load" : unpack('I', b''.join(result[44:48]))[0],                
                }
                del result[:]
                return response
            case "CCCCAAAA":
                print_success("Got device response!")
                response = {
                    "engine_0" : unpack('I', b''.join(result[8:12]))[0],
                    "engine_1" : unpack('I', b''.join(result[12:16]))[0], 
                    "engine_2" : unpack('I', b''.join(result[16:20]))[0], 
                    "engine_3" : unpack('I', b''.join(result[20:24]))[0],            
                }
                del result[:]
                return response
            case _:
                print_fail("Unknown response!")
                # generic = b''.join(result[0:]).decode("utf-8")
                print(result)
                print_warning("Flushing receive buffer...")
                receiver = Thread(target = flush_rx_buffer)
                receiver.start()
                receiver.join()
                flush()

    except:
        print_fail("Unknown response!")
        print(result)
        del result[:]
        # while (result[0] == '0'):
        #     del result[0]
        #     print(result)
        # return process_result()
        # print(result)
        # flush()

def collect_available_packages():
    return os.listdir(package_path)

def upload_binary(package_name):
    print_info("Compile programm...")
    cmd = "make -C " + path_suffix + "packages/" + package_name
    os.system(cmd)

    print_info("Get Size...")
    cmd = "wc -c < " + path_suffix + "packages/" + package_name + "/build/" + package_name + "_binary"
    size = int(os.popen(cmd).read())

    if size > 0:
        print_success("Success - Filesize is : " + hex(size) + " / " + str(size))
    else:
        print_fail("Could not read filesize!")

    text_file = open(path_suffix + "packages/" + package_name + "/prepare", "wb")
    text_file.write(size.to_bytes(4,'big'))
    text_file.close()

    print_info("set device into receiving state...")
    cmd = "printf \"\\x01\\x12\\x34\\x56\" >> /dev/" + device_address
    os.system(cmd)

    time.sleep(1)
    thread_return = False
    receiver = Thread(target = device_rx)
    receiver.start()

    print_info("send binary size to device...")
    cmd = "dd if=" + path_suffix + "packages/" + package_name + "/prepare of=/dev/" + device_address + " iflag=direct,skip_bytes"
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
    cmd = "dd if=" + path_suffix + "packages/" + package_name + "/build/" + package_name + "_binary" + " of=/dev/" + device_address + " iflag=direct,skip_bytes"
    os.system(cmd)   

    print_success("done!")
    flush()

def recompile_binary(final_adress, package_name, size):
    cmd = "cd .."
    os.system(cmd)    
    os.system("pwd")

    final_adress_adjusted = final_adress - 1
    print_warning("Alter adress from " + hex(final_adress) + " to " + hex(final_adress_adjusted))

    # change startadress
    print_info("change startadress in linker script...")
    cmd = "sed -i \"1 s/.*/STARTADRESS\ =\ " + hex(final_adress_adjusted) + ";/\"" + " ../../packages/" + package_name + "/link_external.ld"
    os.system(cmd)

    print_info("recompile programm...")
    cmd = "make -C " + path_suffix + "packages/" + package_name
    os.system(cmd)

@app.route('/runrpm', methods=['GET'])
def runrpm():
    receiver = Thread(target = device_rx)
    receiver.start()

@app.route('/get_packages', methods=['GET'])
def get_packages():
    return collect_available_packages()

def debug1():
    time.sleep(5)
    serial_device.cancel_read()
    print_fail("killed!")

@app.route('/lifetime', methods=['GET'])
def lifetime():
    flush()
    if is_busy:
        return {}
    print_info("set device into REQUEST_STATISTIC state...")
    cmd = "printf \"\\x03\\x12\\x34\\x56\" >> /dev/" + device_address
    os.system(cmd)
    time.sleep(0.05)

    thread_return = False
    receiver = Thread(target = device_rx)
    receiver.start()

    print_info("trigger update...")
    cmd = "printf \"\\x03\" >> /dev/" + device_address
    os.system(cmd)

    print_info("waiting for device response...")
    start = time.time()
    receiver.join()

    # global is_busy
    # now = datetime.datetime.now()
    # # now = time.now()
    # # while (is_busy == True or (datetime.datetime.now()) > timedelta(seconds = 5)):
    # while (is_busy == True):
    #     print("time: ",  is_busy)
    #     pass
    print(is_busy)
    # if not receiver.is_alive():
    #     print_fail("Operation failed!")
    #     return {}
    # if not thread_return:
    #     print_fail("Operation failed!")
    #     return {}

    stop = time.time()
    print_info("Took " + str(stop - start) + " ms")
    response = process_result()
    if response:
        return response
    else:
        return {}

@app.route('/rpm', methods=['GET'])
def rpm():
    print_info("set device into REQUEST_RPM state...")
    cmd = "printf \"\\x05\\x12\\x34\\x56\" >> /dev/" + device_address
    os.system(cmd)
    time.sleep(1)
    
    thread_return = False
    receiver = Thread(target = device_rx)
    receiver.start()

    print_info("trigger update...")
    cmd = "printf \"\\x05\" >> /dev/" + device_address
    os.system(cmd)

    print_info("waiting for device response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    print_info("Took " + str(stop - start) + " ms")
    response = process_result()
    return response    

def alter_speed(speed):
    print_info("set device into speed state...")
    cmd = "printf \"\\x04\\x12\\x34\\x56\" >> /dev/" + device_address
    os.system(cmd) 
    time.sleep(0.2)

    n = list(speed)
    cmd_0 = "printf \"" 
    cmd_1 = "" 
    
    for l in n:
        cmd_1 += "\\\\x" + f'{ord(l):x}' 

    line = cmd_0 + cmd_1 + "\" >> /dev/" + device_address
    os.system(line)

@app.route('/set_speed/<speed>', methods=['POST'])
def set_speed(speed):
    engine_no = int(speed[0])
    new_speed = int(speed[1:4])

    diff = new_speed - current_speed["engine_3"]
    if (diff > 0):
        while current_speed["engine_3"] != new_speed:
            current_speed["engine_3"] += 1
            speed = "3" + str(current_speed["engine_3"])
            if (int(speed) > 300 and int(speed) < 399):
                alter_speed(speed)
                print(speed)
            time.sleep(0.6)
    else :
        while current_speed["engine_3"] != new_speed:
            current_speed["engine_3"] -= 1
            speed = "3" + str(current_speed["engine_3"])
            if (int(speed) > 300 and int(speed) < 399):
                alter_speed(speed)
                print(speed)
            time.sleep(0.6)
    
    current_speed["engine_3"] = int(speed[1:4])
    # alter_speed(speed)
    return 'data'

@app.route('/upload/<package_name>', methods=['POST'])
def upload(package_name):
    flush()
    upload_binary(package_name)
    return 'data'

# @app.route('/updateSpeed', methods=['POST'])
# def updateSpeed():
#     # serial_device.write(b"P")
#     os.system(f"sh -c \'printf \"P\" > {serial_device}\'")
#     time.sleep(0.5)
#     n = 000;
#     # i_r1 = request.json["engine_0"]
#     r1 = f'{int(request.json["engine_0"]):03d}'
#     r2 = f'{int(request.json["engine_1"]):03d}'
#     r3 = f'{int(request.json["engine_2"]):03d}'
#     r4 = f'{int(request.json["engine_3"]):03d}'
#     pwm = r1 + '|' + r2 + '|' +r3 + '|' +r4 + ';'
#     # serial_device.write(str.encode(pwm))
#     cmd = f"sh -c \'printf \"{pwm}\" > {serial_device}\'"
#     print(cmd)
#     os.system(cmd)
#     # print(request.json)
#     return 'data'