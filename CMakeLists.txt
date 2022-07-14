cmake_minimum_required(VERSION 3.22.0)

# Add nano cmake.
execute_process(COMMAND wget -O - https://nano-cmake.onrender.com/nano.cmake OUTPUT_VARIABLE NANO_CMAKE ERROR_QUIET)
cmake_language(EVAL CODE ${NANO_CMAKE})

project(nano-geometry VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(NANO_GEO_BUILD_TESTS "Build tests." OFF)
option(NANO_GEO_DEV "Development build" OFF)

# nano-geometry interface.
nano_create_module(geometry SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nano/geometry.h" DEV)
