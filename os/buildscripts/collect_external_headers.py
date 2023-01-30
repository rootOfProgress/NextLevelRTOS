import os

path_prefix = '../build/memmap.'
public_functions = ['allocate', 'deallocate']
arch = 'qemu'
header = ""

if __name__ == '__main__':
    header += "#ifndef OS_HEADER_H\n"
    header += "#define OS_HEADER_H\n"

    for function in public_functions:
        cmd = "cat " + path_prefix + arch + " | grep -w \"" + function + "\" | tail -n +2 | awk \'{ print $1 }\'"
        # result = os.system(cmd)
        # void (*functionPtr2)(void);
        # functionPtr = (void (*)()) (0x08000843);
        result = int(str(os.popen(cmd).read()).strip(), 16) - 1
        # header += " #define " + function + " " + str(result)

        header += "#define " + function + "_addr " + hex(result) + "\n"

    header += "#endif\n"
    with open("os_header.h", "w") as myfile:
        myfile.write(header)
        myfile.close()

        # print(cmd)
    