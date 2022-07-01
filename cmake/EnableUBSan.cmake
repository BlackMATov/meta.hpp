# https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html

add_library(enable_ubsan INTERFACE)

target_compile_options(enable_ubsan INTERFACE
    -fsanitize=undefined
    -fno-omit-frame-pointer)

target_link_options(enable_ubsan INTERFACE
    -fsanitize=undefined
    -fno-omit-frame-pointer)
