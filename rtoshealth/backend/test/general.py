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

serial_device = None
device_address = ""
mutex = Lock()

def test_general001_lifetime():
    result = os_health.get_lifetime()
    
    if (result["magic"] != 0x12345678):
        return [("Subtest001_000_test_magic", False)]
    
    return [("Subtest001_000_test_magic", True)]
