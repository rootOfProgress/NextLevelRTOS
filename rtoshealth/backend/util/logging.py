from datetime import datetime 

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def print_info(str):
    print("[" + datetime.today().strftime('%H:%M:%S.%f') + "] " + bcolors.OKCYAN + str + bcolors.ENDC)

def print_fail(str):
    print("[" + datetime.today().strftime('%H:%M:%S.%f') + "] " + bcolors.FAIL + str + bcolors.ENDC)

def print_success(str):
    print("[" + datetime.today().strftime('%H:%M:%S.%f') + "] " + bcolors.OKGREEN + str + bcolors.ENDC)

def print_warning(str):
    print("[" + datetime.today().strftime('%H:%M:%S.%f') + "] " + bcolors.WARNING + str + bcolors.ENDC)

def print_bold(str):
    print("[" + datetime.today().strftime('%H:%M:%S.%f') + "] " + bcolors.BOLD + str + bcolors.ENDC)