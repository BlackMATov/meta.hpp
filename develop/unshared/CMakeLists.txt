project(meta.hpp.shared)

# lib

file(GLOB_RECURSE META_SHARED_LIB_SOURCES CONFIGURE_DEPENDS
    "meta_shared_lib/*.cpp"
    "meta_shared_lib/*.hpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${META_SHARED_LIB_SOURCES})

add_library(${PROJECT_NAME}.lib SHARED ${META_SHARED_LIB_SOURCES})
add_library(${PROJECT_NAME}::lib ALIAS ${PROJECT_NAME}.lib)

target_link_libraries(${PROJECT_NAME}.lib PUBLIC
    meta.hpp::meta.hpp
    meta.hpp::setup_targets)

target_include_directories(${PROJECT_NAME}.lib PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/meta_shared_lib)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}.lib)

# exe

file(GLOB_RECURSE META_SHARED_EXE_SOURCES CONFIGURE_DEPENDS
    "meta_shared_exe/*.cpp"
    "meta_shared_exe/*.hpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${META_SHARED_EXE_SOURCES})

add_executable(${PROJECT_NAME}.exe ${META_SHARED_EXE_SOURCES})

target_link_libraries(${PROJECT_NAME}.exe PRIVATE
    ${PROJECT_NAME}::lib
    meta.hpp.vendors::doctest
    meta.hpp.vendors::fmt)

add_test(${PROJECT_NAME}.exe ${PROJECT_NAME}.exe)
