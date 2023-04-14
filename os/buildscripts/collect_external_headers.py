import os
import sys


path_prefix = '../build/memmap.'
public_functions = [
    'allocate',
    'deallocate', 
    'create_task',
    'timer_init'
]
arch = list(sys.argv)[1]
header = ""

if __name__ == '__main__':
    header += "#ifndef OS_HEADER_H\n"
    header += "#define OS_HEADER_H\n"

    for function in public_functions:
        cmd = "cat " + path_prefix + arch + " | grep -w \"" + function + "\" | tail -n +2 | awk \'{ print $1 }\'"
        result = int(str(os.popen(cmd).read()).strip(), 16) | 1

        header += "#define " + function + "_addr " + hex(result) + "\n"

    header += "#endif\n"
    with open("addresses.h", "w") as adress_file:
        adress_file.write(header)
        adress_file.close()
    os.system("cp addresses.h ../../packages/include")
    os.system("cp addresses.h ../../modules/")
    