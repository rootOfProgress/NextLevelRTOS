import json
from flask import Flask
from flask_cors import CORS, cross_origin
from threading import Thread

import os
import time
import serial

# general
app = Flask(__name__)
cors = CORS(app)
app.config['CORS_HEADERS'] = 'Content-Type'
app.config["DEBUG"] = True
import routes.router as router
from threadSync import sync

ID_VENDOR_ID = "067b"
ID_MODEL_ID = "2303"


# SERIAL_DEVICE

# start_new_thread(myThread, ())
def detect_device() -> bool:
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
                ser = serial.Serial(f"/dev/ttyUSB{i}", 9600)
                # router.SERIAL_DEVICE = f"/dev/ttyUSB{i}"
                # sync.SERIAL_DEVICE = f"/dev/ttyUSB{i}"
                router.serial_device = serial.Serial(f"/dev/ttyUSB{i}", 9600, timeout=2, xonxoff=False, rtscts=False, dsrdtr=False) #Tried with and without the last 3 parameters, and also at 1Mbps, same happens.

                # router.serial_device = f"/dev/ttyUSB{i}"
                # sync.serial_device = ser
                # print(router.serial_device)
                return True
    return False


if __name__ == '__main__':
    # receiver = Thread(target = sync.listen_to_device)
    # rpm = Thread(target = router.request_rpm)
    # sync.myThread()
    if detect_device():
        # receiver.start()
        # rpm.start()
        app.run(host="192.168.178.51")
    else:
       print("No Serial Device found!")
    # receiver.join()
