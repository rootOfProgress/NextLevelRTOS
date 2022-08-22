# Details

Date : 2022-02-12 11:36:07

Directory /home/benni/development/os_dev/NextLevelRTOS

Total : 44 files,  1619 codes, 310 comments, 372 blanks, all 2301 lines

[summary](results.md) / details / [diff summary](diff.md) / [diff details](diff-details.md)

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [README.md](/README.md) | Markdown | 0 | 0 | 1 | 1 |
| [cpu/src/core.rs](/cpu/src/core.rs) | Rust | 25 | 6 | 2 | 33 |
| [cpu/src/lib.rs](/cpu/src/lib.rs) | Rust | 2 | 0 | 1 | 3 |
| [devices/src/bus/mod.rs](/devices/src/bus/mod.rs) | Rust | 1 | 0 | 1 | 2 |
| [devices/src/bus/rcc.rs](/devices/src/bus/rcc.rs) | Rust | 32 | 22 | 10 | 64 |
| [devices/src/controller/dma.rs](/devices/src/controller/dma.rs) | Rust | 103 | 5 | 22 | 130 |
| [devices/src/controller/mod.rs](/devices/src/controller/mod.rs) | Rust | 2 | 0 | 1 | 3 |
| [devices/src/controller/uart.rs](/devices/src/controller/uart.rs) | Rust | 70 | 22 | 12 | 104 |
| [devices/src/generic/cpu.rs](/devices/src/generic/cpu.rs) | Rust | 26 | 1 | 4 | 31 |
| [devices/src/generic/mod.rs](/devices/src/generic/mod.rs) | Rust | 3 | 0 | 1 | 4 |
| [devices/src/generic/platform.rs](/devices/src/generic/platform.rs) | Rust | 95 | 4 | 11 | 110 |
| [devices/src/generic/traits.rs](/devices/src/generic/traits.rs) | Rust | 49 | 41 | 6 | 96 |
| [devices/src/io/gpio.rs](/devices/src/io/gpio.rs) | Rust | 164 | 31 | 20 | 215 |
| [devices/src/io/mod.rs](/devices/src/io/mod.rs) | Rust | 1 | 0 | 1 | 2 |
| [devices/src/lib.rs](/devices/src/lib.rs) | Rust | 9 | 0 | 2 | 11 |
| [devices/src/nvic/exti.rs](/devices/src/nvic/exti.rs) | Rust | 45 | 24 | 11 | 80 |
| [devices/src/nvic/mod.rs](/devices/src/nvic/mod.rs) | Rust | 1 | 0 | 1 | 2 |
| [devices/src/registerblocks/dma.rs](/devices/src/registerblocks/dma.rs) | Rust | 29 | 10 | 4 | 43 |
| [devices/src/registerblocks/exti.rs](/devices/src/registerblocks/exti.rs) | Rust | 14 | 6 | 2 | 22 |
| [devices/src/registerblocks/gpio.rs](/devices/src/registerblocks/gpio.rs) | Rust | 19 | 16 | 3 | 38 |
| [devices/src/registerblocks/mod.rs](/devices/src/registerblocks/mod.rs) | Rust | 6 | 0 | 1 | 7 |
| [devices/src/registerblocks/rcc.rs](/devices/src/registerblocks/rcc.rs) | Rust | 42 | 17 | 3 | 62 |
| [devices/src/registerblocks/sys.rs](/devices/src/registerblocks/sys.rs) | Rust | 12 | 10 | 3 | 25 |
| [devices/src/registerblocks/usart.rs](/devices/src/registerblocks/usart.rs) | Rust | 15 | 10 | 2 | 27 |
| [devices/src/sys/mod.rs](/devices/src/sys/mod.rs) | Rust | 1 | 0 | 1 | 2 |
| [devices/src/sys/tick.rs](/devices/src/sys/tick.rs) | Rust | 28 | 2 | 8 | 38 |
| [documentation/part1_boot_procedure.md](/documentation/part1_boot_procedure.md) | Markdown | 285 | 0 | 132 | 417 |
| [documentation/part2_device_access_en.md](/documentation/part2_device_access_en.md) | Markdown | 12 | 0 | 5 | 17 |
| [documentation/part2_device_access_ru.md](/documentation/part2_device_access_ru.md) | Markdown | 14 | 0 | 8 | 22 |
| [kernel/openocd.cfg](/kernel/openocd.cfg) | Properties | 2 | 3 | 2 | 7 |
| [kernel/src/asm/context_mng.s](/kernel/src/asm/context_mng.s) | GAS/AT&T x86/x64 | 25 | 9 | 7 | 41 |
| [kernel/src/data/list.rs](/kernel/src/data/list.rs) | Rust | 80 | 6 | 5 | 91 |
| [kernel/src/data/mod.rs](/kernel/src/data/mod.rs) | Rust | 1 | 0 | 1 | 2 |
| [kernel/src/main.rs](/kernel/src/main.rs) | Rust | 104 | 14 | 18 | 136 |
| [kernel/src/mem/malloc.rs](/kernel/src/mem/malloc.rs) | Rust | 65 | 1 | 8 | 74 |
| [kernel/src/mem/mod.rs](/kernel/src/mem/mod.rs) | Rust | 1 | 0 | 1 | 2 |
| [kernel/src/proc/mod.rs](/kernel/src/proc/mod.rs) | Rust | 3 | 0 | 1 | 4 |
| [kernel/src/proc/sched.rs](/kernel/src/proc/sched.rs) | Rust | 77 | 15 | 18 | 110 |
| [kernel/src/proc/task.rs](/kernel/src/proc/task.rs) | Rust | 8 | 0 | 2 | 10 |
| [kernel/src/proc/tcb.rs](/kernel/src/proc/tcb.rs) | Rust | 6 | 4 | 1 | 11 |
| [process/src/blueprint.rs](/process/src/blueprint.rs) | Rust | 36 | 9 | 7 | 52 |
| [process/src/lib.rs](/process/src/lib.rs) | Rust | 16 | 0 | 2 | 18 |
| [runtime/build.rs](/runtime/build.rs) | Rust | 12 | 4 | 5 | 21 |
| [runtime/src/lib.rs](/runtime/src/lib.rs) | Rust | 78 | 18 | 15 | 111 |

[summary](results.md) / details / [diff summary](diff.md) / [diff details](diff-details.md)