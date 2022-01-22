# 3.12 version is required for `cxx_std_20`
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

if(NOT DEFINED PROJECT_NAME)
    set(BUILD_AS_STANDALONE ON)
else()
    set(BUILD_AS_STANDALONE OFF)
endif()

project(meta.hpp)

add_subdirectory(vendors/enum.hpp)

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE enum.hpp)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_include_directories(${PROJECT_NAME} INTERFACE headers)

target_compile_options(${PROJECT_NAME}
    INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:
        >
    INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:
        >
    INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-ctad-maybe-unsupported
        -Wno-exit-time-destructors
        -Wno-extra-semi-stmt
        -Wno-float-equal
        -Wno-global-constructors
        -Wno-ordered-compare-function-pointers
        -Wno-padded
        -Wno-reserved-identifier
        -Wno-shadow-field
        -Wno-shadow-field-in-constructor
        -Wno-unknown-warning-option
        -Wno-weak-vtables
        -Wno-zero-as-null-pointer-constant
        >)

if(BUILD_AS_STANDALONE)
    enable_testing()
    add_subdirectory(vendors/doctest)
    add_subdirectory(vendors/fmtlib)
    add_subdirectory(manuals)
    add_subdirectory(untests)
endif()
