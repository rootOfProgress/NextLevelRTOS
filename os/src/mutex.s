.global lock_mutex
.global unlock_mutex
.cpu cortex-m4
.syntax unified
.thumb

.EQU locked,1
.EQU unlocked,0

// unlocked  EQU 0

// lock_mutex
// ; Declare for use from C as extern void lock_mutex(void * mutex);
lock_mutex:
    ldr r1, =locked
    ldrex   r2, [r0]
foo:  
    CMP   r2, r1        //; Test if mutex is locked or unlocked
    BEQ    foo           //; If locked - wait for it to be released, from 2
    STREX r2, r1, [r0]  //; Not locked, attempt to lock it
    CMP   r2, #1        //; Check if Store-Exclusive failed
    BEQ     foo           //; Failed - retry from 1
    // Lock acquired
    DMB                   //; Required before accessing protected resource
    BX      lr



// ; unlock_mutex
// ; Declare for use from C as extern void unlock_mutex(void * mutex);
//     EXPORT unlock_mutex
unlock_mutex:
    ldr r1, =unlocked
    DMB                   //; Required before releasing protected resource
    STR     r1, [r0]      //; Unlock mutex
    // SIGNAL_UPDATE       
    BX      lr
//     ENDP
