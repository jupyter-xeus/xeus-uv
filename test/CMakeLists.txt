############################################################################
# Copyright (c) 2026, Dr. Thorsten Beier                                   #
# Copyright (c) 2024, Isabel Paredes                                       #
# Copyright (c) 2024, QuantStack                                           #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################



# add test executable
add_executable(test_kernel 
xmock_interpreter.cpp
main.cpp)

# link to xeus-uv and xeus-zmq
if(XEUS_UV_BUILD_SHARED_LIBS)
    target_link_libraries(test_kernel PRIVATE xeus-uv)
else()
    target_link_libraries(test_kernel PRIVATE xeus-uv-static)
endif()

target_compile_definitions(test_kernel PUBLIC UVW_AS_LIB)


# include current source dir for xmock_interpreter.hpp
target_include_directories(test_kernel PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${UVW_INCLUDE_DIRS}
 )


enable_testing()


find_program(PYTEST  
    NAMES pytest-3 pytest py.test-3 py.test REQUIRED)


configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/test_kernel/kernel.json.in"
    "kernels/test_kernel/kernel.json"
)


configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/test_kernel.py"
    "${CMAKE_CURRENT_BINARY_DIR}/"
    COPYONLY)

add_test(NAME test_kernel
    COMMAND ${PYTEST} test_kernel.py)
set_tests_properties(test_kernel
    PROPERTIES
    ENVIRONMENT "JUPYTER_PATH=${CMAKE_CURRENT_BINARY_DIR}")