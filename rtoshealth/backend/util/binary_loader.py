from __main__ import path_suffix, package_path, logging
# import logging/
import util.uart_receiver as uart_receiver
import os
import time
from threading import Thread, Lock

def recompile_binary(final_adress, package_name, size):
    cmd = "cd .."
    os.system(cmd)    
    os.system("pwd")

    final_adress_adjusted = final_adress - 1
    logging.print_warning("Alter adress from " + hex(final_adress) + " to " + hex(final_adress_adjusted))

    # change startadress
    logging.print_info("change startadress in linker script...")
    cmd = "sed -i \"1 s/.*/STARTADRESS\ =\ " + hex(final_adress_adjusted) + ";/\"" + " ../../packages/" + package_name + "/link_external.ld"
    os.system(cmd)

    logging.print_info("recompile programm...")
    cmd = "make clean -C " + path_suffix + "packages/" + package_name
    os.system(cmd)
    cmd = "make -C " + path_suffix + "packages/" + package_name
    os.system(cmd)

def upload_binary(package_name):
    logging.print_info("Compile programm...")
    cmd = "make -C " + path_suffix + "packages/" + package_name
    os.system(cmd)

    logging.print_info("Get Size...")
    cmd = "wc -c < " + path_suffix + "packages/" + package_name + "/build/" + package_name + "_binary"
    size = int(os.popen(cmd).read())

    if size > 0:
        logging.print_success("Success - Filesize is : " + hex(size) + " / " + str(size))
    else:
        logging.print_fail("Could not read filesize!")

    text_file = open(path_suffix + "packages/" + package_name + "/prepare", "wb")
    text_file.write(size.to_bytes(4,'big'))
    text_file.close()

    logging.print_info("set device into receiving state...")
    cmd = "printf \"\\x01\\x12\\x34\\x56\" >> /dev/" + uart_receiver.device_address
    os.system(cmd)

    # time.sleep(1)
    thread_return = False
    receiver = Thread(target = uart_receiver.device_rx)
    receiver.start()

    logging.print_info("send binary size to device...")
    cmd = "dd if=" + path_suffix + "packages/" + package_name + "/prepare of=/dev/" + uart_receiver.device_address + " iflag=direct,skip_bytes"
    os.system(cmd)
    # time.sleep(1)

    bytes_read = 0
    logging.print_info("waiting for device response...")
    start = time.time()
    
    receiver.join()

    stop = time.time()
    logging.print_info("Took " + str(stop - start) + " ms")

    start_adress = uart_receiver.process_result()
    recompile_binary(start_adress, package_name, size)
    # time.sleep(1)

    logging.print_info("upload binary to target...")
    cmd = "dd if=" + path_suffix + "packages/" + package_name + "/build/" + package_name + "_binary" + " of=/dev/" + uart_receiver.device_address + " iflag=direct,skip_bytes"
    os.system(cmd)   

    logging.print_success("done!")
