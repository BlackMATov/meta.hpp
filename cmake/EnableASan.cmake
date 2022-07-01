# https://clang.llvm.org/docs/AddressSanitizer.html

add_library(enable_asan INTERFACE)

target_compile_options(enable_asan INTERFACE
    -fsanitize=address
    -fno-omit-frame-pointer
    -fsanitize-address-use-after-scope
    -fsanitize-address-use-after-return=always)

target_link_options(enable_asan INTERFACE
    -fsanitize=address
    -fno-omit-frame-pointer
    -fsanitize-address-use-after-scope
    -fsanitize-address-use-after-return=always)
