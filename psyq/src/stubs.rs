//! Calls the games link but do not need yet. Each stub reports itself once
//! on the debug console.

#[allow(unused_macros)]
macro_rules! stub_fn {
    ($name:ident ( $($arg:ident : $ty:ty),* ) $(-> $ret:ty = $val:expr)?) => {
        #[no_mangle]
        pub extern "C" fn $name($($arg: $ty),*) $(-> $ret)? {
            $(let _ = $arg;)*
            stub!(stringify!($name));
            $($val)?
        }
    };
}

#[allow(dead_code)]
type P = *mut u8;

// ------------------------------------------------------------ CD

