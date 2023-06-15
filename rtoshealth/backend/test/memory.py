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

serial_device = None
device_address = ""
mutex = Lock()

def test_memory001_alloc_benchmark():
    global result

    random.seed()
    package_name = "memory_benchmark"
    target_path = path_suffix + "packages/" + package_name + "/include/parameters.h"

    random_numbers = ""

    for r in range(0,32):
        alloc_chunk = random.randrange(256)
        random_numbers += str(alloc_chunk) + ","
      
    sed_command = "sed -i 's/{([0-9]*,)*}/{" + random_numbers + "}/g' " + target_path  
    print(sed_command)         
    # sed_command = "sed -i 's/{([0-9]*,)*[0-9]*,[0-9]*,[0-9]*,}/{" + random_numbers + "}/g' " + target_path           
    # os.system(sed_command)
    # return {}
    test_util.prepare_device()
    binary_loader.upload_binary("memory_benchmark")
    receiver = Thread(target = uart_receiver.device_rx, args=(128,))
    receiver.start()
    logging.print_info("waiting for MEMORY response...")
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

def test_memory002_various():    
    test_util.prepare_device()
    binary_loader.upload_binary("memory_tests")
    receiver = Thread(target = uart_receiver.device_rx, args=(32,))
    receiver.start()
    logging.print_info("waiting for MEMORY response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    logging.print_info("Took " + str(stop - start) + " ms")
    result = uart_receiver.get_rx()
    print(result)
    response = {
        "Subtest002_000_memset_byte" : unpack('b', result[0:1])[0],
        #"Subtest002_001_outofrange" : unpack('b', result[1:2])[0], 
        #"Subtest002_002_alloc_negative" : unpack('b', result[2:3])[0], 
        "reserved" : unpack('@31s', result[1:32])[0],             
    }

    if ((response["Subtest002_000_memset_byte"]) != 1):
        return False
    
    return True
    

def test_memory_api():
    logging.print_info("Reboot OS...")
    os_health.reboot()
    time.sleep(1)
    logging.print_info("Check if OS is alive...")
    lifetime = os_health.get_lifetime()
    if (lifetime == {}):
        logging.print_fail("OS not available")
        return {}
    binary_loader.upload_binary("dummy")
    time.sleep(1)
    test_result = test_util.get_test_results()
    test_result = test_result["test_result"]
    result = []
    for test in range(0,12):
        if (test_result & 1 << test):
            logging.print_success("Test " + str(test) + " succeeded!")
            test = {'test'+str(test): "PASS"}
            result.append(test)
        else:
            logging.print_fail("Test " + str(test) + " failed!")
            test = {'test'+str(test): "FAIL"}
            result.append(test)
    logging.print_info("Check if OS is alive...")
    lifetime = os_health.get_lifetime()
    if (lifetime == {}):
        logging.print_fail("OS not available")
        return {}
    logging.print_info("Reboot OS...")
    os_health.reboot()
    return result

