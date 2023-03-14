add_library(${PROJECT_NAME}.disable_rtti INTERFACE)
add_library(${PROJECT_NAME}::disable_rtti ALIAS ${PROJECT_NAME}.disable_rtti)

target_compile_options(${PROJECT_NAME}.disable_rtti INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -fno-rtti>)
