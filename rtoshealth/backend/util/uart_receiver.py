from __main__ import logging
import json
import os
import time
import serial

# from logging import print_fail, print_info, print_success, print_warning
from threading import Thread, Lock
from struct import pack, unpack
from flask import request
from random import randrange
import datetime, timedelta
# import logging as logging
serial_device = None
result = []
device_address = ""
mutex = Lock()
is_busy = False

def flush_rx_buffer():
    serial_device.flush()
    serial_device.reset_input_buffer()
    serial_device.reset_output_buffer()

def flush():
    del result[:]

def complement(payload):
    return payload / 256
    # # bit 9
    # sign = (payload >> 10) & 1

    # # bit 8 .. 0
    # value = (payload & 0x3FF) * 0.031
    # # negative
    # # if sign == 1:
    # #     return ((512 - value) * -1) / 32
    # # else:
    # #     return value / 32
    # if sign == 1:
    #     return value * -1
    # else:
    #     return value


def process_result():
    # byte 0..7 : code
    bts = b''.join(result[0:8])
    try:
        match bts.decode("utf-8"):
            case "AAAABBBB": #memadress
                start_adress_bin = b''.join(result[8:17])
                start_adress = int(start_adress_bin.decode("utf-8"))
                logging.print_success("Got memory response from device, Startadress is : " + hex(start_adress))
                del result[:]
                return start_adress
            case "BBBBAAAA":
                logging.print_success("Got device response!")
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
                logging.print_success("Got device response!")
                response = {
                    "engine_0" : unpack('I', b''.join(result[8:12]))[0],
                    "engine_1" : unpack('I', b''.join(result[12:16]))[0], 
                    "engine_2" : unpack('I', b''.join(result[16:20]))[0], 
                    "engine_3" : unpack('I', b''.join(result[20:24]))[0],            
                }
                del result[:]
                return response
            case "DDDDAAAA":
                logging.print_success("Got device response!")
                print(result)
                response = {
                    "x" : unpack('<h', b''.join(result[8:10]))[0],
                    "y" : unpack('<h', b''.join(result[10:12]))[0], 
                    "z" : unpack('<h', b''.join(result[12:14]))[0], 
                }
                # response = {
                #     "x_low" : unpack('I', b''.join(result[8:9]))[0],
                #     "y_low" : unpack('I', b''.join(result[9:10]))[0], 
                #     "z_low" : unpack('I', b''.join(result[10:11]))[0],
                #     "x_high" : unpack('I', b''.join(result[11]))[0],
                #     "y_high" : unpack('I', b''.join(result[12]))[0], 
                #     "z_high" : unpack('I', b''.join(result[13]))[0], 
                # }
                # print(response)
                # print("{0:X}".format(response["x_low"]))
                # print("{0:X}".format(response["y_low"]))
                # print("{0:X}".format(response["z_low"]))
                print("{0:016b}".format(response["x"]), response["x"])
                print("{0:016b}".format(response["y"]), response["y"])
                print("{0:016b}".format(response["z"]), response["z"])
                #response["x"] = complement(response["x"])
                #response["y"] = complement(response["y"])
                #response["z"] = complement(response["z"])

                del result[:]
                return response
            case "DDDDEEEE":
                logging.print_success("Got device response!")
                response = {
                    "test_result" : unpack('I', b''.join(result[8:12]))[0]        
                }
                del result[:]
                return response
            case _:
                logging.print_fail("Unknown response!")
                # generic = b''.join(result[0:]).decode("utf-8")
                print(result)
                logging.print_warning("Flushing receive buffer...")
                receiver = Thread(target = flush_rx_buffer)
                receiver.start()
                receiver.join()
                flush()

    except:
        logging.print_fail("Unknown response!")
        print(result)
        del result[:]

def device_rx(expected = 64):
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
            if bytesToRead != 0:
                # print(incoming_byte)
                result.append(incoming_byte)
                bytes_read += 1
                if bytes_read == expected:
                    is_busy = False
                    mutex.release()
                    return
        except:
            pass
            is_busy = False
            mutex.release()
            return