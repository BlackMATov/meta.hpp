add_library(${PROJECT_NAME}.setup_targets INTERFACE)
add_library(${PROJECT_NAME}::setup_targets ALIAS ${PROJECT_NAME}.setup_targets)

target_link_libraries(${PROJECT_NAME}.setup_targets INTERFACE
    doctest::doctest_with_main)

target_compile_options(${PROJECT_NAME}.setup_targets INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:
        /WX /W4>
    $<$<CXX_COMPILER_ID:GNU>:
        -Werror -Wall -Wextra -Wpedantic>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -Werror -Weverything -Wconversion
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-padded
        -Wno-unneeded-internal-declaration
        -Wno-unneeded-member-function
        -Wno-weak-vtables
        >)

if(BUILD_WITH_COVERAGE)
    target_link_libraries(${PROJECT_NAME}.setup_targets INTERFACE
        meta.hpp::enable_gcov)
endif()

if(BUILD_WITH_SANITIZERS)
    target_link_libraries(${PROJECT_NAME}.setup_targets INTERFACE
        meta.hpp::enable_asan
        meta.hpp::enable_ubsan)
endif()
