# https://clang.llvm.org/docs/SourceBasedCodeCoverage.html

add_library(${PROJECT_NAME}.enable_gcov INTERFACE)
add_library(${PROJECT_NAME}::enable_gcov ALIAS ${PROJECT_NAME}.enable_gcov)

target_compile_options(${PROJECT_NAME}.enable_gcov INTERFACE
    --coverage)

target_link_options(${PROJECT_NAME}.enable_gcov INTERFACE
    --coverage)
