cmake_minimum_required(VERSION 3.22.0)

include(FetchContent)

project(nano-geometry VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options.
option(NANO_GEOMETRY_BUILD_TESTS "Build nano-geometry tests." ON)
option(NANO_GEOMETRY_DEV_MODE "Development build" OFF)

# Fetch nano-common.
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../nano-common")
    set(FETCHCONTENT_SOURCE_DIR_NANO_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/../nano-common")
endif()

FetchContent_Declare(nano_common GIT_REPOSITORY "https://github.com/Meta-Sonic/nano-common.git")
FetchContent_MakeAvailable(nano_common)

# Setup nano-geometry sources.
set(NANO_GEOMETRY_NAME geometry)
set(NANO_GEOMETRY_MODULE_NAME nano-${NANO_GEOMETRY_NAME})
set(NANO_GEOMETRY_SRC_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/nano")

file(GLOB_RECURSE NANO_GEOMETRY_SOURCE_FILES
    "${NANO_GEOMETRY_SRC_DIRECTORY}/*.h"
    "${NANO_GEOMETRY_SRC_DIRECTORY}/*.cpp")

# Create nano-geometry interface (nano-geometry or nano::geometry).
add_library(${NANO_GEOMETRY_MODULE_NAME} INTERFACE ${NANO_GEOMETRY_SOURCE_FILES})
add_library(nano::${NANO_GEOMETRY_NAME} ALIAS ${NANO_GEOMETRY_MODULE_NAME})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NANO_GEOMETRY_SOURCE_FILES})

target_include_directories(${NANO_GEOMETRY_MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${NANO_GEOMETRY_MODULE_NAME} PROPERTIES XCODE_GENERATE_SCHEME OFF)
target_link_libraries(${NANO_GEOMETRY_MODULE_NAME} INTERFACE nano::common)

if (NANO_GEOMETRY_DEV_MODE)
    set(NANO_GEOMETRY_BUILD_TESTS ON)
    # nano_clang_format(${NANO_GEOMETRY_MODULE_NAME} ${OPT_SOURCES})
endif()

# Create unit tests executable (nano-geometry-tests).
if (NANO_GEOMETRY_BUILD_TESTS)
    nano_add_module(test)

    set(NANO_GEOMETRY_TESTS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    file(GLOB_RECURSE NANO_GEOMETRY_TESTS_SOURCE_FILES
        "${NANO_GEOMETRY_TESTS_DIRECTORY}/*.cpp"
        "${NANO_GEOMETRY_TESTS_DIRECTORY}/*.h")

    set(NANO_GEOMETRY_TEST_NAME nano-${NANO_GEOMETRY_NAME}-tests)
    add_executable(${NANO_GEOMETRY_TEST_NAME} ${NANO_GEOMETRY_TESTS_SOURCE_FILES})

    source_group(TREE "${NANO_GEOMETRY_TESTS_DIRECTORY}" FILES ${NANO_GEOMETRY_TESTS_SOURCE_FILES})

    target_include_directories(${NANO_GEOMETRY_TEST_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    target_link_libraries(${NANO_GEOMETRY_TEST_NAME} PUBLIC nano::test ${NANO_GEOMETRY_MODULE_NAME})

    set(CLANG_OPTIONS -Weverything -Wno-c++98-compat)
    set(MSVC_OPTIONS /W4)

    target_compile_options(${NANO_GEOMETRY_TEST_NAME}
        PUBLIC
            "$<$<CXX_COMPILER_ID:Clang,AppleClang>:${CLANG_OPTIONS}>"
            "$<$<CXX_COMPILER_ID:MSVC>:${MSVC_OPTIONS}>")
endif()
