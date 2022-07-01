project(meta.hpp.untests)

file(GLOB_RECURSE UNTESTS_SOURCES "*.cpp" "*.hpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${UNTESTS_SOURCES})

add_executable(${PROJECT_NAME} ${UNTESTS_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE meta.hpp doctest_with_main)

if(${BUILD_WITH_COVERAGE})
    target_link_libraries(${PROJECT_NAME} PRIVATE enable_gcov)
endif()

if(${BUILD_WITH_SANITIZERS})
    target_link_libraries(${PROJECT_NAME} PRIVATE enable_asan enable_ubsan)
endif()

target_compile_options(${PROJECT_NAME}
    PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:
        /WX /W4>
    PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:
        -Werror -Wall -Wextra -Wpedantic>
    PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -Werror -Weverything -Wconversion
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-padded
        -Wno-unknown-warning-option
        -Wno-unneeded-internal-declaration
        -Wno-unneeded-member-function
        -Wno-unused-macros
        -Wno-weak-vtables
        >)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS
    DOCTEST_CONFIG_USE_STD_HEADERS)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
