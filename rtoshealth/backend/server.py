import json
from flask import Flask
from flask_cors import CORS, cross_origin
from threading import Thread
import util.logging as logging
# import test.memory as memory
import os
import time
import serial

path_suffix = '../../'
package_path = path_suffix + 'packages'

# general
app = Flask(__name__)
cors = CORS(app)
app.config['CORS_HEADERS'] = 'Content-Type'
app.config["DEBUG"] = True

import routes.router as router
import util.uart_receiver as receiver
from threadSync import sync

ID_VENDOR_ID = "067b"
ID_MODEL_ID = "2303"

def detect_serial_interface() -> bool:
    VENDOR_ID = ""
    MODEL_ID = ""
    for i in range(0,10):
        device_out = os.popen(f"sudo udevadm info --name=/dev/ttyUSB{i}").read()
        if device_out:
            for device_info in device_out.split('\n'):
                if "ID_VENDOR_ID" in device_info:
                    VENDOR_ID = device_info.split('=')[-1]
                if "ID_MODEL_ID" in device_info:
                    MODEL_ID = device_info.split('=')[-1]
            if (VENDOR_ID == ID_VENDOR_ID and MODEL_ID == ID_MODEL_ID):
                print("found device!")
                receiver.serial_device = serial.Serial(f"/dev/ttyUSB{i}", 9600, timeout=20, xonxoff=False, rtscts=False, dsrdtr=False)
                # global device_address
                receiver.device_address = f"ttyUSB{i}"
                return True
    return False

if __name__ == '__main__':
    # receiver = Thread(target = sync.listen_to_device)
    # rpm = Thread(target = router.request_rpm)
    # sync.myThread()
    if detect_serial_interface():
        app.run(host="192.168.178.51")
    else:
       print("No Serial Device found!")
    # receiver.join()
