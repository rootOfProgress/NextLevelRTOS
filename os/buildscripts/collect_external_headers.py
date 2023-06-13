import os
import sys
import git

path_prefix = '../build/memmap.'
public_functions = [
    'allocate',
    'deallocate', 
    'create_task',
    'timer_init',
    'task_sleep',
    'create_task'
]

arch = list(sys.argv)[1]
header = ""

def generate_os_version():
    repo = git.Repo(search_parent_directories=True)
    sha = repo.head.object.hexsha
    sha_short = repo.git.rev_parse(sha, short=8)

    os_version = 12062023

    path = os.getcwd() + "/../include/runtime.h"
    # os.system("cat " + path + "/../include/runtime.h")

    sed_command_githash = "sed -ie '/GIT_HASH/c\#define GIT_HASH 0x" + sha_short + ";'" + " " + path         
    sed_command_osversion = "sed -ie '/OS_VERSION/c\#define OS_VERSION " + str(os_version) + ";'" + " " + path         
    # print(sed_command)
    os.system(sed_command_githash)
    os.system(sed_command_osversion)

    # print(sha)
    # print(repo.git.rev_parse(sha, short=8))

if __name__ == '__main__':

    generate_os_version()

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
    