# 3.12 version is required for `cxx_std_20`
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

if(NOT DEFINED PROJECT_NAME)
    set(BUILD_AS_STANDALONE ON)
else()
    set(BUILD_AS_STANDALONE OFF)
endif()

project(meta.hpp)

add_subdirectory(vendors/enum.hpp)
add_subdirectory(vendors/kari.hpp)

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE enum.hpp kari.hpp)
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
        >)

if(BUILD_AS_STANDALONE)
    option(BUILD_WITH_UNTESTS "Build with unit tests" ON)
    if(BUILD_WITH_UNTESTS)
        enable_testing()
        add_subdirectory(vendors/doctest)
        add_subdirectory(untests)
    endif()
endif()
