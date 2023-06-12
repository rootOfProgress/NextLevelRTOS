from __main__ import app, path_suffix, package_path, logging

import json
import os
import time
import serial
import test.memory
import test.scheduler
import test.test_main
import util.uart_receiver as uart_receiver
import util.binary_loader as binary_loader
import util.os_health as os_health
import test.test_main as test_main
import test.test_util as test_util
import test.memory as memory_test
import test.scheduler as scheduler_test

# from logging import print_fail, print_info, print_success, print_warning
from threading import Thread, Lock
from struct import pack, unpack
from flask import request
from random import randrange
import datetime, timedelta
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

@app.route('/runrpm', methods=['GET'])
def runrpm():
    receiver = Thread(target = device_rx)
    receiver.start()

@app.route('/get_packages', methods=['GET'])
def get_packages():
    return os.listdir(package_path)

def debug1():
    time.sleep(5)
    serial_device.cancel_read()
    logging.print_fail("killed!")

@app.route('/lifetime', methods=['GET'])
def lifetime():
    return os_health.get_lifetime()

@app.route('/reboot', methods=['GET'])
def reboot():
    os_health.reboot()
    return {}    

@app.route('/get_test_results', methods=['GET'])
def get_test_results():
    return test_util.get_test_results()

@app.route('/test/all', methods=['GET'])
def test_all():
    return test_main.run_all()    

@app.route('/test/memory', methods=['GET'])
def test_memory():
    return memory_test.test_memory001_alloc_benchmark()

@app.route('/test/scheduler', methods=['GET'])
def test_scheduler():
    return scheduler_test.test_scheduler002_contextswitch_benchmark()

@app.route('/test/sleep', methods=['GET'])
def test_sleep():
    return scheduler_test.test_scheduler001_sleep_benchmark()

@app.route('/test/sleep_load', methods=['GET'])
def test_sleep_load():
    return scheduler_test.test_scheduler001_sleep_benchmark(True)

@app.route('/rpm', methods=['GET'])
def rpm():
    logging.print_info("set device into REQUEST_RPM state...")
    cmd = "printf \"\\x05\\x12\\x34\\x56\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)
    time.sleep(1)
    
    thread_return = False
    receiver = Thread(target = uart_receiver.device_rx)
    receiver.start()

    logging.print_info("trigger update...")
    cmd = "printf \"\\x05\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)

    logging.print_info("waiting for device response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    logging.print_info("Took " + str(stop - start) + " ms")
    response = process_result()
    return response    

@app.route('/position', methods=['GET'])
def position():
    logging.print_info("set device into REQUEST_POSITION state...")
    cmd = "printf \"\\x08\\x12\\x34\\x56\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)
    time.sleep(1)
    
    thread_return = False
    receiver = Thread(target = uart_receiver.device_rx)
    receiver.start()

    logging.print_info("trigger update...")
    cmd = "printf \"\\x08\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)

    logging.print_info("waiting for device response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    logging.print_info("Took " + str(stop - start) + " ms")
    response = uart_receiver.process_result()
    return response    

def alter_speed(speed):
    logging.print_info("set device into speed state...")
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
    binary_loader.upload_binary(package_name)
    return 'data'
