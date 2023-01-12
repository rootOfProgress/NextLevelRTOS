// source: https://docs.rust-embedded.org/embedonomicon/main.html
use std::{env, error::Error, fs::File, io::Write, path::PathBuf};

use cc::Build;

fn main() -> Result<(), Box<dyn Error>> {
    // build directory for this crate
    let out_dir = PathBuf::from(env::var_os("OUT_DIR").unwrap());

    // extend the library search path
    println!("cargo:rustc-link-search={}", out_dir.display());

    // put `link.x` in the build directory
    File::create(out_dir.join("../link_external.ld"))?.write_all(include_bytes!("../link_external.ld"))?;
    Ok(())
}
//legacy_drivers