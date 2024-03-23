import os
import sys
import git
import subprocess

path_prefix = '../build/memmap.'
public_functions = [
    'allocate',
    'deallocate', 
    'create_task',
    'timer_init',
    'task_sleep',
    'create_task',
    'join_task',
    'memset_byte',
    'link_exti_src',
    'execute_priviledged',
    'link_adc_src',
    'lock_mutex',
    'release_mutex'
]

arch = list(sys.argv)[1]
header = ""

def generate_os_version():
    # collect git revision
    repo = git.Repo(search_parent_directories=True)
    sha = repo.head.object.hexsha
    sha_short = repo.git.rev_parse(sha, short=8)

    # collect os version 
    os_version = 23032024

    # collect gcc version
    raw_output = subprocess.check_output(['arm-none-eabi-gcc', '--version'])
    raw_output_split_by_line = raw_output.decode('utf-8').split('\n')
    raw_output_split_by_whitespace = raw_output_split_by_line[0].split(' ')
    gcc_version = raw_output_split_by_whitespace[3].replace('.','')

    # write into os header
    path = os.getcwd() + "/../include/runtime.h"
    sed_command_githash = "sed -i '/GIT_HASH/c\#define GIT_HASH 0x" + sha_short + ";'" + " " + path         
    sed_command_osversion = "sed -i '/OS_VERSION/c\#define OS_VERSION " + str(os_version) + ";'" + " " + path         
    sed_command_gccversion = "sed -i '/GCC_VERSION/c\#define GCC_VERSION " + str(gcc_version) + ";'" + " " + path         

    os.system(sed_command_githash)
    os.system(sed_command_osversion)
    os.system(sed_command_gccversion)

if __name__ == '__main__':

    generate_os_version()

    header += "#ifndef OS_HEADER_H\n"
    header += "#define OS_HEADER_H\n"

    for function in public_functions:
        cmd = "cat " + path_prefix + arch + " | grep -w \"" + function + "\" | tail -n +2 | awk \'{ print $1 }\'"
        result = 0

        try:
            result = int(str(os.popen(cmd).read()).strip(), 16) | 1
        except:
            cmd = "cat " + path_prefix + arch + " | grep -w \"" + function + "\" | tail -n +1 | awk \'{ print $1 }\'"
            result = int(str(os.popen(cmd).read()).strip(), 16) | 1
        header += "#define " + function + "_addr " + hex(result) + "\n"

    header += "#endif\n"
    with open("addresses.h", "w") as adress_file:
        adress_file.write(header)
        adress_file.close()
    os.system("cp addresses.h ../../packages/include")
    os.system("cp addresses.h ../../modules/")
    
