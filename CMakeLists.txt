cmake_minimum_required(VERSION 3.15)
project(my_mini_infer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(mini_infer
    src/tensor.cpp
)

target_include_directories(mini_infer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(mini_infer_app
    src/main.cpp
)
target_link_libraries(mini_infer_app PRIVATE mini_infer)

enable_testing()

add_executable(test_tensor
    tests/test_tensor.cpp
)
target_link_libraries(test_tensor PRIVATE mini_infer)

add_test(NAME test_tensor COMMAND test_tensor)
