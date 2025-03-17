############################################################################
# Copyright (c) 2024, Isabel Paredes                                       #
# Copyright (c) 2024, QuantStack                                           #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

# Unit tests
# ==========

cmake_minimum_required(VERSION 3.10)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(xeus-uv-test)

    enable_testing()

    find_package(xeus-uv REQUIRED CONFIG)
    find_package(nlohmann_json QUIET CONFIG)
    find_package(xeus-zmq REQUIRED)

    set(XEUS_UV_TEST_DIR ${CMAKE_CURRENT_LIST_DIR})
endif ()

message(STATUS "Forcing tests build type to Release")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

if(nlohmann_json_FOUND)
  add_definitions(-DHAVE_NLOHMANN_JSON)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES Clang OR CMAKE_CXX_COMPILER_ID MATCHES GNU OR CMAKE_CXX_COMPILER_ID MATCHES Intel)
    add_compile_options(-Wunused-parameter -Wextra -Wreorder -Wconversion -Wsign-conversion)

    CHECK_CXX_COMPILER_FLAG(-march=native HAS_MARCH_NATIVE)
    if (HAS_MARCH_NATIVE)
        add_compile_options(-march=native)
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    add_compile_options(/EHsc /MP /bigobj)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

find_program(PYTEST NAMES pytest-3 pytest py.test-3 py.test REQUIRED)

if(nlohmann_json_FOUND)
    # Version up to 3.1.2 export the target `nlohmann_json`
    if(TARGET nlohmann_json)
      set(nlohmann_json_TARGET nlohmann_json)
    # Newer versions export the namespaced target `nlohmann_json::nlohmann_json`
    elseif(TARGET nlohmann_json::nlohmann_json)
      set(nlohmann_json_TARGET nlohmann_json::nlohmann_json)
    endif()
endif()

find_library(LIBUV_LIBRARIES NAMES uv libuv)
find_path(LIBUV_INCLUDE_DIR NAMES uv.h)
message(STATUS "Found libuv: ${LIBUV_LIBRARIES}")

find_library(UVW_LIBRARY NAMES uvw libuvw)
find_path(UVW_INCLUDE_DIR uvw.hpp)
message(STATUS "Found uvw: ${UVW_LIBRARY}")

find_package(xeus-zmq REQUIRED)

if (TARGET xeus-uv)
    set(xeus-uv_TARGET xeus-uv)
    message(STATUS "Found xeus-uv shared library.")
elseif (TARGET xeus-uv-static)
    set(xeus-uv_TARGET xeus-uv-static)
    message(STATUS "Found xeus-uv static library.")
endif ()

# Test_kernel tests
# =======================

set(TEST_KERNEL_SOURCES
    xhook.cpp
    xhook.hpp
    xmock_interpreter.cpp
    xmock_interpreter.hpp
    main.cpp)

configure_file(
    "${XEUS_UV_TEST_DIR}/test_kernel/kernel.json.in"
    "kernels/test_kernel/kernel.json"
)

add_executable(test_kernel ${TEST_KERNEL_SOURCES})
target_link_libraries(
    test_kernel
    PUBLIC ${xeus-uv_TARGET} ${LIBUV_LIBRARIES} ${UVW_LIBRARY} xeus-zmq
)

target_include_directories(
    test_kernel
    PRIVATE ${LIBUV_INCLUDE_DIR} ${UVW_INCLUDE_DIR}
)

target_compile_features(test_kernel PRIVATE cxx_std_17)
target_compile_definitions(test_kernel PUBLIC UVW_AS_LIB)

set(CONNECTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/connection.json)

configure_file(
    "${XEUS_UV_TEST_DIR}/test_kernel.py"
    "${CMAKE_CURRENT_BINARY_DIR}/"
    COPYONLY)

add_test(NAME test_kernel
    COMMAND ${PYTEST} test_kernel.py)
set_tests_properties(test_kernel
    PROPERTIES
    ENVIRONMENT "JUPYTER_PATH=${CMAKE_CURRENT_BINARY_DIR}")
