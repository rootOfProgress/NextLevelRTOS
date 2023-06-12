from __main__ import path_suffix, package_path, logging
import util.uart_receiver as uart_receiver
import os
import time
from threading import Thread, Lock

def reboot():
    logging.print_info("set device into REBOOT state...")
    cmd = "printf \"\\x06\\x56\\x34\\x12\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)
    
    logging.print_info("perform reboot...")
    cmd = "printf \"\\x06\\x56\\x34\\x12\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)

def get_lifetime():
    logging.print_info("Request lifetime info...")
    cmd = "printf \"\\x03\\x56\\x34\\x12\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)

    receiver = Thread(target = uart_receiver.device_rx, args=(40,))
    receiver.start()
    cmd = "printf \"\\x03\\x56\\x34\\x12\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)

    logging.print_info("Waiting for device response...")
    start = time.time()
    receiver.join()

    stop = time.time()
    response = uart_receiver.process_result("lifetime")
    if response:
        return response
    else:
        return {}