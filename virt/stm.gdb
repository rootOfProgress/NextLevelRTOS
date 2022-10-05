set style address foreground yellow 
set style function foreground green
set style variable foreground cyan
target extended-remote :1234
# print demangled symbols
set print asm-demangle on

# set backtrace limit to not have infinite backtrace loops
set backtrace limit 32
break exception.c:32
#break main.c:77
# start the process but immediately halt the processor
set print pretty on

