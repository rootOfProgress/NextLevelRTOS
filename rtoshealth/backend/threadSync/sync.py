# -*- coding: utf-8 -*-
import os
from __main__ import app

# global
pwm_values = {}
state = "P"

# import serial
@app.route('/getSpeed', methods=['GET'])
def getSpeed():
    data = {
        "a": 0,
        "b": 2
    }
    return data


def update(payload):
    print("payload " + payload)
    match state:
        case "P":
            global pwm_values
            pwm_values = {
                "0": payload[0]
            }

# SERIAL_DEVICE = ""
serial_device = None
# ser = serial.Serial('/dev/ttyUSB1', 9600)

def init_serial():
    return serial.Serial(SERIAL_DEVICE, 9600)

def listen_to_device():
    import time

    while not serial_device:
        pass

    # serial_device = init_serial()

    while True:
        device_out = serial_device.readline()
        result = device_out.decode()
        match result:
            case "P":
                # global state
                state = "P"
            case "I":
                # global state
                state = "I"
            case "R":
                # global state
                state = "R"
            case _:
                update(result)
        # print(device_out.decode())
        #except:
        #    pass
        # device_out = ser.readline()
        # time.sleep(3)