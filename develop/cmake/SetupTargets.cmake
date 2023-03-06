add_library(${PROJECT_NAME}.setup_targets INTERFACE)
add_library(${PROJECT_NAME}::setup_targets ALIAS ${PROJECT_NAME}.setup_targets)

target_compile_options(${PROJECT_NAME}.setup_targets INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:
        /WX /W4 /bigobj>
    $<$<CXX_COMPILER_ID:GNU>:
        -Werror -Wall -Wextra -Wpedantic
        -Wno-inaccessible-base>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -Werror -Weverything -Wconversion
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-padded
        -Wno-unneeded-internal-declaration
        -Wno-unused-macros
        -Wno-unused-member-function
        -Wno-weak-vtables
        -Wno-zero-as-null-pointer-constant
        >)

target_link_libraries(${PROJECT_NAME}.setup_targets INTERFACE
    $<$<BOOL:${BUILD_WITH_COVERAGE}>:
        meta.hpp::enable_gcov>
    $<$<BOOL:${BUILD_WITH_SANITIZERS}>:
        meta.hpp::enable_asan
        meta.hpp::enable_ubsan>
    $<$<BOOL:${BUILD_WITH_NO_EXCEPTIONS}>:
        meta.hpp::disable_exceptions>
    $<$<BOOL:${BUILD_WITH_NO_RTTI}>:
        meta.hpp::disable_rtti>)

target_compile_definitions(${PROJECT_NAME}.setup_targets INTERFACE
    $<$<BOOL:${BUILD_WITH_SANITIZERS}>:META_HPP_SANITIZERS>)
