import os
import sys
import json

path_prefix = '../build/memmap.'

external_header = "#ifndef EXTERNALS_H\n#define EXTERNALS_H\n\n"

func_ptr_list = "static void (*func_ptr[NUM_OF_EXTERNAL_FUNCTIONS])() = {"
extern_declarations = ""


if __name__ == '__main__':
    dir_of_invokation = list(sys.argv)[1]
    module_list = open(dir_of_invokation + "/../modules/mod.json")
    data = json.load(module_list)

    package_list="PACKAGES="
    activated_mods=0
    for module_name, is_activated in data.items():
        if is_activated:
            package_list += " " + module_name
            activated_mods += 1
            func_ptr_list += module_name + ","
            # extern_declarations += "void __attribute__((__noipa__))  __attribute__((optimize(\"O0\"))) " + module_name + "(void);\n"
            extern_declarations += "extern void " + module_name + "(void);\n"

        # os.system("sed -i '1c" + package_list + "'" +  dir_of_invokation + "/../modules/Makefile")
    # print("sed -i '1c" + package_list + "' " +  dir_of_invokation + "/../modules/Makefile")
    func_ptr_list += "};"

    external_header += "#define NUM_OF_EXTERNAL_FUNCTIONS " + str(activated_mods) + "\n"
    external_header += '\n'
    external_header += extern_declarations
    external_header += '\n'
    external_header += func_ptr_list
    external_header += "\n\n#endif"
    
    print(external_header)
    f = open("externals.h", "w")
    f.write(external_header)
    f.close()
    os.system("mv externals.h ./include")
    os.system("sed -i '1c" + package_list + "' " +  dir_of_invokation + "/../modules/Makefile")
    print(package_list)
    