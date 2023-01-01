project(meta.hpp.manuals)

file(GLOB_RECURSE MANUALS_SOURCES "*.cpp" "*.hpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MANUALS_SOURCES})

add_executable(${PROJECT_NAME} ${MANUALS_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE meta.hpp::meta.hpp)

add_executable(${PROJECT_NAME}.singles ${MANUALS_SOURCES})
target_link_libraries(${PROJECT_NAME}.singles PRIVATE meta.hpp::singles)

#
# setup defines
#

function(setup_defines_for_target TARGET)
    target_compile_definitions(${TARGET} PRIVATE
        DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS
        DOCTEST_CONFIG_USE_STD_HEADERS)
endfunction()

setup_defines_for_target(${PROJECT_NAME})
setup_defines_for_target(${PROJECT_NAME}.singles)

#
# setup libraries
#

function(setup_libraries_for_target TARGET)
    target_link_libraries(${TARGET} PRIVATE doctest::doctest_with_main)

    if(BUILD_WITH_COVERAGE)
        target_link_libraries(${TARGET} PRIVATE meta.hpp::enable_gcov)
    endif()

    if(BUILD_WITH_SANITIZERS)
        target_link_libraries(${TARGET} PRIVATE meta.hpp::enable_asan meta.hpp::enable_ubsan)
    endif()
endfunction()

setup_libraries_for_target(${PROJECT_NAME})
setup_libraries_for_target(${PROJECT_NAME}.singles)

#
# setup warnings
#

function(setup_warnings_for_target TARGET)
    target_compile_options(${TARGET}
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
endfunction()

setup_warnings_for_target(${PROJECT_NAME})
setup_warnings_for_target(${PROJECT_NAME}.singles)

#
# add tests
#

add_test(${PROJECT_NAME} ${PROJECT_NAME})
add_test(${PROJECT_NAME} ${PROJECT_NAME}.singles)
