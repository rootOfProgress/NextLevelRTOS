from __main__ import path_suffix, package_path, logging
# import logging/
import util.uart_receiver as uart_receiver
import os
import time
from threading import Thread, Lock

def reboot():
    logging.print_info("set device into REBOOT state...")
    cmd = "printf \"\\x06\\x12\\x34\\x56\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)
    time.sleep(1)
    
    logging.print_info("perform reboot...")
    cmd = "printf \"\\x06\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)

def get_lifetime():
    uart_receiver.flush()
    if uart_receiver.is_busy:
        return {}
    logging.print_info("set device into REQUEST_STATISTIC state...")
    cmd = "printf \"\\x03\\x12\\x34\\x56\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)
    time.sleep(0.05)

    receiver = Thread(target = uart_receiver.device_rx)
    receiver.start()

    logging.print_info("trigger update...")
    cmd = "printf \"\\x03\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)

    logging.print_info("waiting for device response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    logging.print_info("Took " + str(stop - start) + " ms")
    response = uart_receiver.process_result()
    if response:
        return response
    else:
        return {}