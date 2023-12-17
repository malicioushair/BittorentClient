cmake_minimum_required(VERSION 3.13)
project(bittorrent)
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
     message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
     file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
     "${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(
     CONANFILE conanfile.txt
     BASIC_SETUP
     BUILD missing
)

include_directories(${PROJECT_SOURCE_DIR}/src)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

