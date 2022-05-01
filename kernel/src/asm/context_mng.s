.global __save_process_context
.global __load_process_context
.global __br_to_init
.global __trap
.global __br_to
.global __get_r0_r1
.global __branch
.cpu cortex-m4
.syntax unified
.thumb

__get_r0_r1:
	bx lr

__trap:
	svc 0
	bx lr

__br_to_init:
	ldmfd r0!, {r4-r11}
	msr psp, r0
	mov lr, #0xfffffffd
    bx lr

// __br_to:
// 	ldmfd r0!, {r4-r11}
// 	msr psp, r0
// 	mov lr, #0xfffffffd
//     bx lr

// __br_to_init:
// 	svc 0
// 	mov lr, #0xfffffffd
//     bx lr


__save_process_context:
	mrs r0, psp
	stmdb r0!, {r4-r11}
	msr psp, r0
	bx lr

__load_process_context:
	ldmfd r0!, {r4-r11}
	msr psp, r0
	bx lr