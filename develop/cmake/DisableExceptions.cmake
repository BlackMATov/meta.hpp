add_library(${PROJECT_NAME}.disable_exceptions INTERFACE)
add_library(${PROJECT_NAME}::disable_exceptions ALIAS ${PROJECT_NAME}.disable_exceptions)

target_compile_options(${PROJECT_NAME}.disable_exceptions INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -fno-exceptions>)
