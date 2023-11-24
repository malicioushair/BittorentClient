cmake_minimum_required(VERSION 3.13)
project(bittorrent)
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard
set(SOURCE_FILES 
    src/lib/nlohmann/json.hpp
    src/Main.cpp
    src/Decoder.h
    src/Decoder.cpp
)
add_executable(bittorrent ${SOURCE_FILES})
