project(meta.hpp.untests)

file(GLOB_RECURSE UNTESTS_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${UNTESTS_SOURCES})

add_executable(${PROJECT_NAME} ${UNTESTS_SOURCES})
add_executable(${PROJECT_NAME}.singles ${UNTESTS_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    meta.hpp::meta.hpp
    meta.hpp::setup_targets
    meta.hpp.vendors::doctest)

target_link_libraries(${PROJECT_NAME}.singles PRIVATE
    meta.hpp::singles
    meta.hpp::setup_targets
    meta.hpp.vendors::doctest)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    META_HPP_HEADERS_BUILD)

target_compile_definitions(${PROJECT_NAME}.singles PRIVATE
    META_HPP_SINGLES_BUILD)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
add_test(${PROJECT_NAME} ${PROJECT_NAME}.singles)
