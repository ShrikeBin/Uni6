/*
 * The Rust library already exports C-ABI symbols via #[no_mangle] extern "C".
 * This file is a documentation/pass-through wrapper.
 */
#include "rust_wrapper.h"
/* All symbols are linked directly from libmathlib (Rust). No body needed. */