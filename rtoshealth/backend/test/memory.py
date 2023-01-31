from __main__ import path_suffix, package_path, logging

import json
import os
import time
import serial
from threading import Thread, Lock
from struct import pack, unpack
from flask import request
from random import randrange
import datetime, timedelta
serial_device = None
result = []
device_address = ""
mutex = Lock()


def test_memory_api():
    upload_binary("dummy")
    return

