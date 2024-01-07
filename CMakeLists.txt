cmake_minimum_required(VERSION 3.13)
project(bittorrent)
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

execute_process(
    COMMAND conan install ${CMAKE_SOURCE_DIR} --build=missing --output-folder=${CMAKE_BINARY_DIR} -s build_type=Debug
)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${PROJECT_SOURCE_DIR}/src)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

