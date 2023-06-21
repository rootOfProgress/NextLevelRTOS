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

def test_nrf_000_various():    
    test_util.prepare_device()
    binary_loader.upload_binary("nrf2401_test")
    receiver = Thread(target = uart_receiver.device_rx, args=(32,))
    receiver.start()
    logging.print_info("waiting for NRF2401 response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    logging.print_info("Took " + str(stop - start) + " ms")
    result = uart_receiver.get_rx()
    print(result)    
    response = {
        "Subtest000_000_check_default_config" : unpack('b', result[0:1])[0],
        "Subtest000_001_check_default_status" : unpack('b', result[1:2])[0], 
        "Subtest000_002_check_power_on" : unpack('b', result[2:3])[0], 
        "Subtest000_002_check_tx_addr_rw" : unpack('b', result[3:4])[0], 
    }
    processed_results = []

    for r in response:
        processed_results.append((r, response[r]))
        
    return processed_results
