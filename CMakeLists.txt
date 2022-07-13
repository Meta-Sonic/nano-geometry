cmake_minimum_required(VERSION 3.22.0)

# No reason to set CMAKE_CONFIGURATION_TYPES if it's not a multiconfig generator.
get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (IS_MULTI_CONFIG)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
    set(CMAKE_XCODE_GENERATE_SCHEME ON)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
else()
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release"  CACHE STRING "Build type" FORCE)
    endif()
endif()

project(nano-geometry VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

option(NANO_GEO_BUILD_TESTS "Build tests." OFF)
option(NANO_GEO_DEV "Development build" OFF)

# nano-test interface.
set(NANO_GEO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nano/geometry.h")
add_library(${PROJECT_NAME} INTERFACE ${NANO_GEO_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NANO_GEO_SOURCES})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(nano::geometry ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES XCODE_GENERATE_SCHEME OFF)

if (NANO_GEO_DEV)
    set(NANO_GEO_BUILD_TESTS ON)
    find_program(CLANG_FORMAT clang-format REQUIRED)
    add_custom_target(check-nano-geo-formatting DEPENDS ${NANO_GEO_SOURCES})
    add_custom_command(TARGET check-nano-geo-formatting PRE_BUILD
        COMMAND "${CLANG_FORMAT}" --Werror  --dry-run ${NANO_GEO_SOURCES}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    add_dependencies(${PROJECT_NAME} check-nano-geo-formatting)
    set_target_properties(check-nano-geo-formatting PROPERTIES XCODE_GENERATE_SCHEME OFF)
endif()

if (NANO_GEO_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(nano-test
        GIT_REPOSITORY https://github.com/Meta-Sonic/nano-test.git
        GIT_TAG master)

    FetchContent_MakeAvailable(nano-test)

    file(GLOB_RECURSE TEST_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h")

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/tests" FILES ${TEST_SOURCE_FILES})

    add_executable(nano-geometry-tests ${TEST_SOURCE_FILES})
    target_include_directories(nano-geometry-tests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    target_link_libraries(nano-geometry-tests PUBLIC nano::test nano::geometry)

    set(CLANG_OPTIONS -Weverything -Wno-c++98-compat)
    set(MSVC_OPTIONS /W4)

    target_compile_options(nano-geometry-tests PUBLIC
        "$<$<CXX_COMPILER_ID:Clang,AppleClang>:${CLANG_OPTIONS}>"
        "$<$<CXX_COMPILER_ID:MSVC>:${MSVC_OPTIONS}>")

    # set_target_properties(nano-geometry-tests PROPERTIES CXX_STANDARD 20)
endif()
