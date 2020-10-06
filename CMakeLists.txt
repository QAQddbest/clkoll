cmake_minimum_required(VERSION 3.17)

project(clkoll C)

set(CMAKE_C_STANDARD 99)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(client)

add_subdirectory(server)