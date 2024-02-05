find_package(Doxygen REQUIRED)

set(DOXYGEN_EXCLUDE_SYMBOLS "meta_hpp::detail")
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_RECURSIVE NO)

set(META_HPP_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(META_HPP_DOXYGEN_INPUT_DIR "${META_HPP_ROOT_DIR}/headers/meta.hpp")

doxygen_add_docs(
    ${PROJECT_NAME}.doxygen ALL
    ${META_HPP_DOXYGEN_INPUT_DIR}
    WORKING_DIRECTORY ${META_HPP_DOXYGEN_INPUT_DIR}
    COMMENT "Generating API documentation with Doxygen")
