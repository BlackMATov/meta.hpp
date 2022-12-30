# https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html

add_library(${PROJECT_NAME}.enable_ubsan INTERFACE)
add_library(${PROJECT_NAME}::enable_ubsan ALIAS ${PROJECT_NAME}.enable_ubsan)

target_compile_options(${PROJECT_NAME}.enable_ubsan INTERFACE
    -fsanitize=undefined
    -fno-omit-frame-pointer)

target_link_options(${PROJECT_NAME}.enable_ubsan INTERFACE
    -fsanitize=undefined
    -fno-omit-frame-pointer)
