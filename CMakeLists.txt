############################################################################
# Copyright (c) 2024, Isabel Paredes                                       #
# Copyright (c) 2024, QuantStack                                           #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.10)
project(xeus-uv)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
set(XEUS_UV_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(XEUS_UV_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(XEUS_UV_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

# Versioning
# ===========

# Project version
file(STRINGS "${XEUS_UV_INCLUDE_DIR}/xeus-uv/xeus-uv.hpp" xeus_uv_version_defines
     REGEX "#define XEUS_UV_VERSION_(MAJOR|MINOR|PATCH)")
foreach(ver ${xeus_uv_version_defines})
    if(ver MATCHES "#define XEUS_UV_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XEUS_UV_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(XEUS_UV_VERSION
    ${XEUS_UV_VERSION_MAJOR}.${XEUS_UV_VERSION_MINOR}.${XEUS_UV_VERSION_PATCH})
message(STATUS "xeus-uv version: v${XEUS_UV_VERSION}")

# Binary version
# See the following URL for explanations about the binary versioning
# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html#Updating-version-info
file(STRINGS "${XEUS_UV_INCLUDE_DIR}/xeus-uv/xeus-uv.hpp" xeus_uv_version_defines
    REGEX "#define XEUS_UV_BINARY_(CURRENT|REVISION|AGE)")
foreach(ver ${xeus_uv_version_defines})
    if(ver MATCHES "#define XEUS_UV_BINARY_(CURRENT|REVISION|AGE) +([^ ]+)$")
        set(XEUS_UV_BINARY_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(XEUS_UV_BINARY_VERSION
    ${XEUS_UV_BINARY_CURRENT}.${XEUS_UV_BINARY_REVISION}.${XEUS_UV_BINARY_AGE})
message(STATUS "xeus-uv binary version: v${XEUS_UV_BINARY_VERSION}")

# Build options
# =============

# Compilation options
option(XEUS_UV_BUILD_SHARED_LIBS "Build xeus shared library." ON)
option(XEUS_UV_BUILD_STATIC_LIBS "Build xeus static library (default if BUILD_SHARED_LIBS is OFF)." ON)
option(XEUS_UV_STATIC_DEPENDENCIES "link statically with xeus dependencies" OFF)

# Test options
option(XEUS_UV_BUILD_TESTS "xeus test suite" OFF)

# Static build configuration
# ==========================

if(XEUS_UV_STATIC_DEPENDENCIES)
    set(CPPZMQ_TARGET_NAME cppzmq-static)
    set(XEUS_TARGET_NAME xeus-static)
    set(OPENSSL_USE_STATIC_LIBS ON CACHE BOOL "Linking statically with OpenSSL")
else()
    set(CPPZMQ_TARGET_NAME cppzmq)
    set(XEUS_TARGET_NAME xeus)
    set(OPENSSL_USE_STATIC_LIBS OFF CACHE BOOL "Not linking statically with OpenSSL")
endif()

# Print build configuration
# ==========================

message(STATUS "XEUS_UV_BUILD_SHARED_LIBS:          ${XEUS_UV_BUILD_SHARED_LIBS}")
message(STATUS "XEUS_UV_BUILD_STATIC_LIBS:          ${XEUS_UV_BUILD_STATIC_LIBS}")
message(STATUS "XEUS_UV_STATIC_DEPENDENCIES:        ${XEUS_UV_STATIC_DEPENDENCIES}")
message(STATUS "XEUS_UV_BUILD_TESTS:                ${XEUS_UV_BUILD_TESTS}")

# Dependencies
# ============

set(xeus_REQUIRED_VERSION 4.0.3)

if(NOT TARGET xeus AND NOT TARGET xeus-static)
    find_package(xeus ${xeus_REQUIRED_VERSION} REQUIRED)
    message(STATUS "Found xeus ${xeus_VERSION}")
endif()

find_library(UVW_LIBRARY uvw)
find_path(UVW_INCLUDE_DIR uvw.hpp)

# Source files
# ============

set(XEUS_UV_HEADERS
    ${XEUS_UV_INCLUDE_DIR}/xeus-uv/xeus-uv.hpp
    ${XEUS_UV_INCLUDE_DIR}/xeus-uv/xhook_base.hpp
)

set(XEUS_UV_SOURCES
    ${XEUS_UV_SOURCE_DIR}/xshell_uv.hpp
    ${XEUS_UV_SOURCE_DIR}/xshell_uv.cpp
)

# Targets and link
# ================

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

macro(xeus_uv_create_target target_name linkage output_name)
    string(TOUPPER "${linkage}" linkage_upper)

    if(NOT ${linkage_upper} MATCHES "^(SHARED|STATIC)$")
        message(FATAL_ERROR "Invalid library linkage: ${linkage}")
    endif()

    # Output
    # ======

    add_library(${target_name} ${linkage_upper} ${XEUS_UV_SOURCES} ${XEUS_UV_HEADERS})

    target_include_directories(
        ${target_name}
        PUBLIC $<BUILD_INTERFACE:${XEUS_UV_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    )

    target_link_libraries(
        ${target_name}
        PUBLIC ${XEUS_TARGET_NAME}
        PUBLIC ${UVW_LIBRARY}
    )

    if(APPLE)
        target_link_libraries(${target_name} PUBLIC "-framework CoreFoundation")
    endif()

    if(UNIX)
        # CMake does not compute the version number of so files as libtool
        # does on Linux. Strictly speaking, we should exclude FreeBSD and
        # Apple from this, but that would require having different version
        # numbers depending on the platform. We prefer to follow the
        # libtool pattern everywhere.
        math(EXPR XEUS_UV_BINARY_COMPATIBLE "${XEUS_UV_BINARY_CURRENT} - ${XEUS_UV_BINARY_AGE}")
        set_target_properties(
            ${target_name}
            PROPERTIES
            PUBLIC_HEADER "${XEUS_UV_HEADERS}"
            COMPILE_OPTIONS "-fvisibility=hidden"
            COMPILE_DEFINITIONS "XEUS_UV_EXPORTS"
            PREFIX ""
            VERSION
            "${XEUS_UV_BINARY_COMPATIBLE}.${XEUS_UV_BINARY_REVISION}.${XEUS_UV_BINARY_AGE}"
            SOVERSION ${XEUS_UV_BINARY_COMPATIBLE}
            OUTPUT_NAME "lib${output_name}"
        )
    else()
        set_target_properties(
            ${target_name}
            PROPERTIES
            PUBLIC_HEADER "${XEUS_UV_HEADERS}"
            COMPILE_DEFINITIONS "XEUS_UV_EXPORTS"
            PREFIX ""
            VERSION ${XEUS_UV_BINARY_VERSION}
            SOVERSION ${XEUS_UV_BINARY_CURRENT}
            OUTPUT_NAME "lib${output_name}"
        )
    endif()

    # Compilation flags
    # =================

    target_compile_features(${target_name} PRIVATE cxx_std_17)
    target_compile_definitions(${target_name} PUBLIC UVW_AS_LIB)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
        CMAKE_CXX_COMPILER_ID MATCHES "Intel")

        target_compile_options(${target_name} PUBLIC -Wunused-parameter -Wextra -Wreorder)

        message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
    endif()

    if(${linkage_upper} STREQUAL "STATIC")
        target_compile_definitions(${target_name} PUBLIC XEUS_UV_STATIC_LIB)
    endif()

    if(MSVC)
        target_compile_definitions(${target_name} PUBLIC -DNOMINMAX)
        target_compile_options(${target_name} PUBLIC /DGUID_WINDOWS /MP /bigobj)
        target_compile_options(${target_name} PUBLIC /wd4251 /wd4996)
    elseif(APPLE)
        target_compile_definitions(${target_name} PUBLIC -DGUID_CFUUID)
    else ()
        target_compile_definitions(${target_name} PUBLIC -DGUID_LIBUUID)
    endif()

    if(XEUS_UV_STATIC_DEPENDENCIES)
        if(CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS})
        endif()
        if(UNIX AND NOT APPLE)
            target_link_libraries(${target_name} PRIVATE util rt)
        endif()
    endif()
endmacro()

set(xeus_uv_targets "")

if(XEUS_UV_BUILD_SHARED_LIBS)
    xeus_uv_create_target(xeus-uv SHARED xeus-uv)
    list(APPEND xeus_uv_targets xeus-uv)
endif()

if(XEUS_UV_BUILD_STATIC_LIBS)
    # On Windows, a static library should use a different output name
    # to avoid the conflict with the import library of a shared one.
    if(CMAKE_HOST_WIN32)
        xeus_uv_create_target(xeus-uv-static STATIC xeus-uv-static)
    else ()
        xeus_uv_create_target(xeus-uv-static STATIC xeus-uv)
    endif()

    list(APPEND xeus_uv_targets xeus-uv-static)
endif()

# Tests
# =====

# We need to control from outside whether we enable testing or not. We cannot
# rely on BUILD_TESTING since it does not exist until CTest is included.

include(CTest)

if(XEUS_UV_BUILD_TESTS)
    set(BUILD_TESTING ON)
    message(STATUS "tests enabled")
else ()
    set(BUILD_TESTING OFF)
    message(STATUS "tests disabled")
endif()

if(BUILD_TESTING)
    add_subdirectory(test)
endif()

# Installation
# ============

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(XEUS_UV_CMAKECONFIG_INSTALL_DIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xeus-uvConfig.cmake")

install(TARGETS ${xeus_uv_targets}
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xeus-uv)

# Makes the project importable from the build directory
export(EXPORT ${PROJECT_NAME}-targets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

# Configure 'xeusConfig.cmake' for a build tree
set(XEUS_UV_CONFIG_CODE "####### Expanded from \@XEUS_CONFIG_CODE\@ #######\n")
set(XEUS_UV_CONFIG_CODE "${XEUS_UV_CONFIG_CODE}set(CMAKE_MODULE_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/cmake;\${CMAKE_MODULE_PATH}\")\n")
set(XEUS_UV_CONFIG_CODE "${XEUS_UV_CONFIG_CODE}##################################################")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${PROJECT_BINARY_DIR})

# Configure 'xeusConfig.cmake' for an install tree
set(XEUS_UV_CONFIG_CODE "")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${XEUS_UV_CMAKECONFIG_INSTALL_DIR})


write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${XEUS_UV_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
              DESTINATION ${XEUS_UV_CMAKECONFIG_INSTALL_DIR})
install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION ${XEUS_UV_CMAKECONFIG_INSTALL_DIR})
