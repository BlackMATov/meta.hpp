# https://clang.llvm.org/docs/SourceBasedCodeCoverage.html

add_library(enable_gcov INTERFACE)

target_compile_options(enable_gcov INTERFACE
    --coverage)

target_link_options(enable_gcov INTERFACE
    --coverage)
