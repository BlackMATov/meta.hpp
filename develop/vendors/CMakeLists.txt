project(meta.hpp.vendors)

#
# doctest
#

add_library(${PROJECT_NAME}.doctest STATIC EXCLUDE_FROM_ALL doctest/doctest/parts/doctest.cpp)
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

add_library(${PROJECT_NAME}.fmt INTERFACE EXCLUDE_FROM_ALL)
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
add_library(${PROJECT_NAME}.gbench STATIC EXCLUDE_FROM_ALL ${GBENCH_SOURCES})
add_library(${PROJECT_NAME}::gbench ALIAS ${PROJECT_NAME}.gbench)

target_compile_features(${PROJECT_NAME}.gbench
    PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME}.gbench SYSTEM
    PUBLIC gbench/include)

target_compile_definitions(${PROJECT_NAME}.gbench
    PUBLIC BENCHMARK_STATIC_DEFINE)

target_link_libraries(${PROJECT_NAME}.gbench
    PRIVATE $<$<PLATFORM_ID:Windows>:shlwapi>)

#
# vmath.hpp
#

add_library(${PROJECT_NAME}.vmath.hpp INTERFACE EXCLUDE_FROM_ALL)
add_library(${PROJECT_NAME}::vmath.hpp ALIAS ${PROJECT_NAME}.vmath.hpp)

target_compile_features(${PROJECT_NAME}.vmath.hpp
    INTERFACE cxx_std_20)

target_include_directories(${PROJECT_NAME}.vmath.hpp SYSTEM
    INTERFACE vmath.hpp/headers)
