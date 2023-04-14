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
    print(bcolors.OKCYAN + str + bcolors.ENDC)

def print_fail(str):
    print(bcolors.FAIL + str + bcolors.ENDC)

def print_success(str):
    print(bcolors.OKGREEN + str + bcolors.ENDC)

def print_warning(str):
    print(bcolors.WARNING + str + bcolors.ENDC)