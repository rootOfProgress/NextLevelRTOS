//!
//! Contains the first entrypoint for startup procedure.
//! Only for runtime purposes, acts as a platform for the kernel
//! code build on top of it.
//!
#![no_std]
#![feature(asm)]
use core::panic::PanicInfo;
use core::ptr;
use core::arch::asm;
use core::cell::Cell;

extern "C" {
    fn __br_to_init();
}

///
/// Mandatory resetfunction at adress 0x08000004.
/// Gets called after power on the cpu.
/// source: https://docs.rust-embedded.org/embedonomicon/main.html
///
#[no_mangle]
pub unsafe extern "C" fn Reset() -> ! {
    extern "C" {
        static mut _sbss: u8;
        static mut _ebss: u8;

        static mut _sdata: u8;
        static mut _edata: u8;
        static _sidata: u8;
    }

    let count = &_ebss as *const u8 as usize - &_sbss as *const u8 as usize;
    ptr::write_bytes(&mut _sbss as *mut u8, 0, count);

    let count = &_edata as *const u8 as usize - &_sdata as *const u8 as usize;
    ptr::copy_nonoverlapping(&_sidata as *const u8, &mut _sdata as *mut u8, count);

    extern "Rust" {
        fn kernel_init(_edata: Cell<u32>) -> !;
    }
    kernel_init(Cell::new(&_edata as *const u8 as u32));
}

pub union Vector {
    reserved: u32,
    handler: unsafe extern "C" fn(),
}

pub union VectorDivergentFn {
    _reserved: u32,
    handler: unsafe extern "C" fn() -> !,
}


extern "C" {
    fn SysTick();
    fn SVCall();
    fn Usart1_MainISR();
    fn MemoryManagementFault();
}


///
/// Manually create a section with points to the adress of
/// the reset function.
///
#[link_section = ".vector_table.reset"]
#[no_mangle]
pub static RESET: [VectorDivergentFn; 1] = [VectorDivergentFn { handler: Reset }];

#[link_section = ".vector_table.exceptions"]
#[no_mangle]
pub static EXCEPTIONS: [Vector; 53] = [
    Vector { reserved: 0 }, // 2
    Vector { handler: HardFaultHandler },
    Vector { handler: MemoryManagementFault },
    Vector { reserved: 0 }, // 5 
    Vector { reserved: 0 },
    Vector { reserved: 0 },
    Vector { reserved: 0 }, // 8
    Vector { reserved: 0 },
    Vector { reserved: 0 },
    Vector { handler: SVCall }, // 11
    Vector { reserved: 0 },
    Vector { reserved: 0 },
    Vector { reserved: 0 },
    Vector { handler: SysTick }, // pos. 15 abs., 
    Vector { reserved: 0 }, // wwdg,  pos. 0 relative, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { reserved: 0 }, 
    Vector { handler: Usart1_MainISR }, 
    Vector { reserved: 0 }, 
];

//-----------------------------------------------------------------//
//------------------------EXCEPTION-HANDLER------------------------//
//-----------------------------------------------------------------//


#[no_mangle]
pub extern "C" fn HardFaultHandler() {
    loop{}
}

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {

        if let Some(s) = _panic.payload().downcast_ref::<&str>() {
            //println!("panic occurred: {s:?}");
            loop {}

        } else {
            //println!("panic occurred");
            loop {}
        }

}
