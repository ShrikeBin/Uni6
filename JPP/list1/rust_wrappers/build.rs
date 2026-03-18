fn main() {
    // Link the C static library
    println!("cargo:rustc-link-search=native=../c_lib");
    println!("cargo:rustc-link-lib=static=mathlib_c");

    // Link the Ada static library (gnat runtime also needed)
    println!("cargo:rustc-link-search=native=../ada_lib");
    println!("cargo:rustc-link-lib=static=mathlib_ada");
    println!("cargo:rustc-link-lib=gnat");
}