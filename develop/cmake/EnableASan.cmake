# https://clang.llvm.org/docs/AddressSanitizer.html

add_library(${PROJECT_NAME}.enable_asan INTERFACE)
add_library(${PROJECT_NAME}::enable_asan ALIAS ${PROJECT_NAME}.enable_asan)

target_compile_options(${PROJECT_NAME}.enable_asan INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -fsanitize=address
        -fno-omit-frame-pointer
        -fsanitize-address-use-after-scope
        -fsanitize-address-use-after-return=always>)

target_link_options(${PROJECT_NAME}.enable_asan INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -fsanitize=address
        -fno-omit-frame-pointer
        -fsanitize-address-use-after-scope
        -fsanitize-address-use-after-return=always>)
