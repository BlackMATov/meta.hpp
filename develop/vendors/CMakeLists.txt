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
    PRIVATE DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
    INTERFACE DOCTEST_CONFIG_USE_STD_HEADERS)

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
