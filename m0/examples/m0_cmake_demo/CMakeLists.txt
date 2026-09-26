cmake_minimum_required(VERSION 3.15)
project(m0_cmake_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(m0_hello main.cpp)