from __main__ import path_suffix, package_path, logging

import json
import os
import time
import serial
from threading import Thread, Lock
from struct import pack, unpack
import util.binary_loader as binary_loader
import util.os_health as os_health
import test.test_util as test_util
import util.uart_receiver as uart_receiver
import random


def test_scheduler001_sleep_benchmark(load = False):
    global result

    random.seed()
    package_name = "sleep_benchmark"
    target_path = path_suffix + "packages/" + package_name + "/include/parameters.h"

    expected_sleep_time = []
    random_numbers = ""

    for r in range(0,4):
        sleeptime = random.randrange(2000)
        
        # convert to usec as result will be in usec
        expected_sleep_time.append(sleeptime * 1000)
        random_numbers += str(sleeptime) + ","
      
    sed_command = "sed -i 's/{[0-9]*,[0-9]*,[0-9]*,[0-9]*,}/{" + random_numbers + "}/g' " + target_path           
    os.system(sed_command)
   
    test_util.prepare_device()

    if load:
        for _ in range(0,4):
            binary_loader.upload_binary("empty_unterminated")

    binary_loader.upload_binary("sleep_benchmark")
    receiver = Thread(target = uart_receiver.device_rx, args=(16,))
    receiver.start()
    logging.print_info("waiting for measurement results response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    logging.print_info("Took " + str(stop - start) + " ms")
    response = uart_receiver.get_rx()
    results_as_array = []

    j = 0
    for i in range(0,16,4):
        actual_sleep_time = unpack('<I', response[i:i+4])[0]
        diff = (actual_sleep_time - expected_sleep_time[j]) / (expected_sleep_time[j] / 100)
        results_as_array.append(str(expected_sleep_time[j]) + " | " + str(actual_sleep_time) + " diff: " + str(diff) + "%")
        j += 1

    response_to_client = {
        "raw": results_as_array
    }
    return response_to_client

def test_scheduler002_sleep_benchmark_load():
    return test_scheduler001_sleep_benchmark(True)

def test_scheduler003_contextswitch_benchmark():
    global result
    test_util.prepare_device()
    binary_loader.upload_binary("scheduler_benchmark")
    receiver = Thread(target = uart_receiver.device_rx, args=(128,))
    receiver.start()
    logging.print_info("waiting for measurement results response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    logging.print_info("Took " + str(stop - start) + " ms")
    response = uart_receiver.get_rx()
    results_as_array = []
    for i in range(0,128,4):
        results_as_array.append(unpack('<I', response[i:i+4])[0])
    
    response_to_client = {
        "min": min(results_as_array),
        "avg": sum(results_as_array) / len(results_as_array),
        "max": max(results_as_array),
        "raw": results_as_array
    }
    return response_to_client

def test_scheduler004_finish_task():
    result = os_health.get_lifetime()
    finished_tasks_pre = result["finished_tasks"]

    binary_loader.upload_binary("empty")

    time.sleep(2)
    result = os_health.get_lifetime()
    finished_tasks_post = result["finished_tasks"]

    if ((finished_tasks_post - finished_tasks_pre) != 1):
        return False
    
    return True

def test_scheduler005_join_task():
    binary_loader.upload_binary("memory_tests")
    receiver = Thread(target = uart_receiver.device_rx, args=(32,))
    receiver.start()
    logging.print_info("waiting for MEMORY response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    logging.print_info("Took " + str(stop - start) + " ms")
    result = uart_receiver.get_rx()

    response = {
        "Subtest005_000_join_task" : unpack('b', result[0:1])[0], 
        "reserved" : unpack('@31s', result[1:32])[0],             
    }

    if ((response["Subtest005_000_join_task"]) != 1):
        return False
    
    return True
    