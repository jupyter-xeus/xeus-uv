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

    set(XEUS_UV_TEST_DIR ${CMAKE_CURRENT_LIST_DIR})
endif ()
