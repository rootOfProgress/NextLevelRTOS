set style address foreground yellow 
set style function foreground green
set style variable foreground cyan
target extended-remote :1234
# print demangled symbols
set print asm-demangle on

# set backtrace limit to not have infinite backtrace loops
set backtrace limit 32

#break main.c:60
#break main.rs:187
#break task.rs:23
#break task.rs:35
#break sched.rs:31
#break list.rs:55


# start the process but immediately halt the processor
set print pretty on

