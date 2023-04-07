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
result = b''
device_address = ""
mutex = Lock()
is_busy = False

def flush_rx_buffer():
    serial_device.flush()
    serial_device.reset_input_buffer()
    serial_device.reset_output_buffer()

def flush():
    serial_device.read(42)

def process_result(result_type = ""):
    # bts = result[0:8].decode("utf-8")
    # print(bts)
    match result_type:
        case "memadress": 
            start_adress = unpack('<I', result[0:4])[0]
            logging.print_success("Got memory response from device, Startadress is : " + hex(start_adress))
            return start_adress
        case "lifetime":
            print(result)
            print(result[24:28])
            print(unpack('I', result[24:28])[0])
            print(len(result))

            logging.print_success("Got device response for Lifetime!")
            response = {
                "num_of_allocs" : unpack('<I', result[0:4])[0],
                "num_of_deallocs" : unpack('<I', result[4:8])[0], 
                "ram_size" : unpack('<I', result[8:12])[0], 
                "total_byte_alloced" : unpack('<I', result[12:16])[0], 
                "total_byte_used" : unpack('<I', result[16:20])[0], 
                "os_data_end" : unpack('I', result[20:24])[0], 
                "free_useable" : unpack('I', result[24:28])[0], 
                "waiting_tasks" : unpack('<I', result[28:32])[0], 
                "running_tasks" : unpack('<I', result[32:36])[0], 
                "cpu_load" : unpack('I', result[36:40])[0],              
            }
            # response = {
            #     "num_of_allocs" : unpack('I', result[0:4])[0],
            #     "num_of_deallocs" : unpack('I', result[4:8])[0], 
            #     "ram_size" : unpack('I', result[8:12])[0], 
            #     "total_byte_alloced" : unpack('I', result[12:16])[0], 
            #     "total_byte_used" : unpack('I', result[16:20])[0], 
            #     "os_data_end" : unpack('I', result[20:24])[0], 
            #     "free_useable" : unpack('I', result[24:28])[0], 
            #     "waiting_tasks" : unpack('I', result[28:32])[0], 
            #     "running_tasks" : unpack('I', result[32:36])[0], 
            #     "cpu_load" : unpack('I', result[36:40])[0],                
            # }
            return response
        case "CCCCAAAA":
            logging.print_success("Got device response!")
            response = {
                "engine_0" : unpack('I', result[8:12])[0],
                "engine_1" : unpack('I', result[12:16])[0], 
                "engine_2" : unpack('I', result[16:20])[0], 
                "engine_3" : unpack('I', result[20:24])[0],            
            }
            return response
        case "DDDDAAAA":
            logging.print_success("Got device response!")
            print(result)
            response = {
                "x" : unpack('<h', result[8:10])[0],
                "y" : unpack('<h', result[10:12])[0], 
                "z" : unpack('<h', result[12:14])[0], 
            }
            print("{0:016b}".format(response["x"]), response["x"])
            print("{0:016b}".format(response["y"]), response["y"])
            print("{0:016b}".format(response["z"]), response["z"])
            del result[:]
            return response
        case "DDDDEEEE":
            logging.print_success("Got device response!")
            response = {
                "test_result" : unpack('I', result[8:12])[0]        
            }
            return response
        case _:
            logging.print_fail("Unknown response!")
            print(result)
            logging.print_warning("Flushing receive buffer...")
            receiver = Thread(target = flush_rx_buffer)
            receiver.start()
            receiver.join()
            flush()

def device_rx(expected = 1):
    global result
    global mutex
    global is_busy
    mutex.acquire()
    incoming_data = serial_device.read(expected)
    result = incoming_data
    mutex.release()
