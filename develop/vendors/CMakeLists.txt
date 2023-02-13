project(meta.hpp.vendors)

#
# doctest
#

add_library(${PROJECT_NAME}.doctest STATIC doctest/doctest/parts/doctest.cpp)
add_library(${PROJECT_NAME}::doctest ALIAS ${PROJECT_NAME}.doctest)

target_compile_features(${PROJECT_NAME}.doctest
    PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME}.doctest SYSTEM
    PUBLIC doctest)

target_compile_definitions(${PROJECT_NAME}.doctest
    PRIVATE
        DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
    PUBLIC
        DOCTEST_CONFIG_USE_STD_HEADERS
        $<$<BOOL:${BUILD_WITH_NO_EXCEPTIONS}>:
            DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS>)

#
# fmt
#

add_library(${PROJECT_NAME}.fmt INTERFACE)
add_library(${PROJECT_NAME}::fmt ALIAS ${PROJECT_NAME}.fmt)

target_compile_features(${PROJECT_NAME}.fmt
    INTERFACE cxx_std_20)

target_include_directories(${PROJECT_NAME}.fmt SYSTEM
    INTERFACE fmt/include)

target_compile_definitions(${PROJECT_NAME}.fmt
    INTERFACE FMT_HEADER_ONLY)

#
# gbench
#

file(GLOB_RECURSE GBENCH_SOURCES CONFIGURE_DEPENDS "gbench/src/*.cc")
add_library(${PROJECT_NAME}.gbench STATIC ${GBENCH_SOURCES})
add_library(${PROJECT_NAME}::gbench ALIAS ${PROJECT_NAME}.gbench)

target_compile_features(${PROJECT_NAME}.gbench
    PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME}.gbench SYSTEM
    PUBLIC gbench/include)

target_compile_definitions(${PROJECT_NAME}.gbench
    PUBLIC BENCHMARK_STATIC_DEFINE)

#
# rttr
#

file(GLOB_RECURSE RTTR_SOURCES CONFIGURE_DEPENDS "rttr/src/rttr/*.cpp")
add_library(${PROJECT_NAME}.rttr STATIC ${RTTR_SOURCES})
add_library(${PROJECT_NAME}::rttr ALIAS ${PROJECT_NAME}.rttr)

target_compile_features(${PROJECT_NAME}.rttr
    PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME}.rttr SYSTEM
    PUBLIC rttr/src)

configure_file(
    "rttr/src/rttr/detail/base/version.h.in"
    "generated/vendors/rttr/rttr/detail/base/version.h"
    @ONLY)

target_include_directories(${PROJECT_NAME}.rttr SYSTEM
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/generated/vendors/rttr")

#
# vmath.hpp
#

add_library(${PROJECT_NAME}.vmath.hpp INTERFACE)
add_library(${PROJECT_NAME}::vmath.hpp ALIAS ${PROJECT_NAME}.vmath.hpp)

target_compile_features(${PROJECT_NAME}.vmath.hpp
    INTERFACE cxx_std_20)

target_include_directories(${PROJECT_NAME}.vmath.hpp SYSTEM
    INTERFACE vmath.hpp/headers)
