.global read
.global bar
.cpu cortex-m4
.syntax unified
.thumb

read:
    ldr r0, [r0, #0]
    bx lr

bar:
    str r0, [r1]
    bx lr
