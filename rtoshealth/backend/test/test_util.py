from __main__ import path_suffix, package_path, logging


import util.uart_receiver as uart_receiver
import os
import time
from threading import Thread, Lock

def get_test_results():
    logging.print_info("set device into REQUEST_TEST_RESULT state...")
    cmd = "printf \"\\x07\\x12\\x34\\x56\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)
    time.sleep(1)
    
    thread_return = False
    receiver = Thread(target = uart_receiver.device_rx, args=(12,))
    receiver.start()

    logging.print_info("trigger update...")
    cmd = "printf \"\\x07\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)

    logging.print_info("waiting for device response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    logging.print_info("Took " + str(stop - start) + " ms")
    response = uart_receiver.process_result()
    return response 