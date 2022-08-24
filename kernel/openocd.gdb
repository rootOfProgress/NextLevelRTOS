target extended-remote :3333
#monitor reset halt
#load
#continue

#break uart.rs:178
#commands
#silent
#p buffer
#cont

break uart.rs:152
commands
silent
p res
cont
