from __main__ import path_suffix, package_path, logging


import util.uart_receiver as uart_receiver
import os
import time
from threading import Thread, Lock
import util.os_health as os_health

def prepare_device():
    os_health.reboot()
    time.sleep(1)
    logging.print_info("Check if OS is alive...")
    lifetime = os_health.get_lifetime()
    if (lifetime == {}):
        logging.print_fail("OS not available")
        return {}
    return lifetime

