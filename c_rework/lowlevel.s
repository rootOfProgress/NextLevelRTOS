.global __save_process_context
.global __load_process_context
.global __br_to_init
.global __trap
.global __br_to
.global __branch
.cpu cortex-m4
.syntax unified
.thumb

__trap:
	svc 0
