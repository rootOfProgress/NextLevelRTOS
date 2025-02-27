.global lock_mutex
.global release_mutex
.cpu cortex-m4
.syntax unified
.thumb

.EQU locked,1
.EQU unlocked,0

lock_mutex:
    ldrex   r2, [r0]
    CMP   r2, locked        //; Test if mutex is locked or unlocked
    BEQ     lock_mutex      //; If locked - wait for it to be released, from 2
    STREX r2, r1, [r0]  //; Not locked, attempt to lock it
    CMP   r2, #1        //; Check if Store-Exclusive failed
    BEQ     lock_mutex           //; Failed - retry from 1
    DMB                   //; Required before accessing protected resource
    BX      lr

release_mutex:
    ldr r1, =unlocked
    DMB                   //; Required before releasing protected resource
    STR     r1, [r0]      //; Unlock mutex
    BX      lr
