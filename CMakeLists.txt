# 3.8 version is required for `cxx_std_17`
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

if(NOT DEFINED PROJECT_NAME)
    set(BUILD_AS_STANDALONE ON)
endif()

project(meta.hpp)

add_subdirectory(vendors/enum.hpp)
add_subdirectory(vendors/kari.hpp)

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE enum.hpp kari.hpp)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_include_directories(${PROJECT_NAME} INTERFACE headers)

target_compile_options(${PROJECT_NAME}
    INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -Wno-c++98-compat-pedantic
        -Wno-covered-switch-default
        -Wno-ctad-maybe-unsupported
        -Wno-exit-time-destructors
        -Wno-padded
        -Wno-poison-system-directories
        -Wno-shadow
        -Wno-unknown-warning-option
        -Wno-weak-vtables>)

if(BUILD_AS_STANDALONE)
    option(BUILD_WITH_UNTESTS "Build with unit tests" ON)
    if(BUILD_WITH_UNTESTS)
        enable_testing()
        add_subdirectory(untests)
    endif()
endif()
