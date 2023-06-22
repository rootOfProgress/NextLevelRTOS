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

def test_nrf_001_getconfig():    
    test_util.prepare_device()
    binary_loader.upload_binary("nrf2401_test")
    receiver = Thread(target = uart_receiver.device_rx, args=(47,))
    receiver.start()
    logging.print_info("waiting for NRF2401 CONFIG response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    logging.print_info("Took " + str(stop - start) + " ms")
    result = uart_receiver.get_rx()
    response = {
        "config" : hex(unpack('b', result[0:1])[0]),
        "en_aa" : hex(unpack('b', result[1:2])[0]),
        "en_rxaddr" : hex(unpack('b', result[2:3])[0]),
        "setup_aw" : hex(unpack('b', result[3:4])[0]),
        "setup_retr" : hex(unpack('b', result[4:5])[0]),
        "rf_ch" : hex(unpack('b', result[5:6])[0]),
        "rf_setup" : hex(unpack('b', result[6:7])[0]),
        "status" : hex(unpack('b', result[7:8])[0]),
        "observe_tx" : hex(unpack('b', result[8:9])[0]),
        "rpd" : hex(unpack('b', result[9:10])[0]),
        "rx_addr_p0" : hex(unpack('q', result[10:18])[0]),
        "rx_addr_p1" : hex(unpack('q', result[18:26])[0]),
        "rx_addr_p2" : hex(unpack('B', result[26:27])[0]),
        "rx_addr_p3" : hex(unpack('B', result[27:28])[0]),
        "rx_addr_p4" : hex(unpack('B', result[28:29])[0]),
        "rx_addr_p5" : hex(unpack('B', result[29:30])[0]),
        "tx_addr" : hex(unpack('q', result[30:38])[0]),
        "rx_pw_p0" : hex(unpack('b', result[38:39])[0]),
        "rx_pw_p1" : hex(unpack('b', result[39:40])[0]),
        "rx_pw_p2" : hex(unpack('b', result[40:41])[0]),
        "rx_pw_p3" : hex(unpack('b', result[41:42])[0]),
        "rx_pw_p4" : hex(unpack('b', result[42:43])[0]),
        "rx_pw_p5" : hex(unpack('b', result[43:44])[0]),
        "fifo_status" : hex(unpack('b', result[44:45])[0]),
        "dyndp" : hex(unpack('b', result[45:46])[0]),
        "feature" : hex(unpack('b', result[46:47])[0]),
    }
 
    return response
