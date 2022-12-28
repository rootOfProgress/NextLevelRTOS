import serial
import struct

ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=2, xonxoff=False, rtscts=False, dsrdtr=False) #Tried with and without the last 3 parameters, and also at 1Mbps, same happens.
# ser = serial.Serial('/tmp/s9.out', 9600, timeout=2, xonxoff=False, rtscts=False, dsrdtr=False) #Tried with and without the last 3 parameters, and also at 1Mbps, same happens.

ser.flushInput()
ser.flushOutput()

result = []

def process_result():
    bts = b''.join(result[0:8])
    match bts.decode("utf-8"):
        case "AAAABBBB":
            start_adress_bin = b''.join(result[8:17])
            start_adress = int(start_adress_bin.decode("utf-8"))
            print(hex(start_adress))
        case _:
            generic = b''.join(result[0:]).decode("utf-8")
            print(generic)

    del result[:]
    # bts = b''.join(result)
    # bts_string =  bts.decode("utf-8")
    # print(bts)

# def process_result():
#     del result[-1]

#     bts = b''.join(result)
#     bts_string =  bts.decode("utf-8")

#     try:
#         print(type(bts_string))
#         bts_int = int(bts.decode("utf-8"))
#         print(hex(bts_int))
#     except:
#         print("not a int")
#         print(bts.decode("utf-8"))

#     # foo = struct.pack(result)

#     # print(result.join(b'', result).decode('ascii'))
#     # print(type(result))
#     del result[:]

bytes_read = 0
while True:
    bytesToRead = ser.inWaiting()
    incoming_byte = ser.read(bytesToRead)
    if bytesToRead != 0:
        result.append(incoming_byte)
        bytes_read += 1
        if bytes_read == 64:
            process_result()
            bytes_read = 0
        # print(result)