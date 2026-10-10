cmake_minimum_required(VERSION 3.15)
project(my_mini_infer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(mini_infer
    src/tensor.cpp
    src/mlp.cpp
    src/tensor_io.cpp
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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/catch2")
include(Catch)

add_executable(test_tensor
    tests/test_tensor.cpp
)
target_link_libraries(test_tensor PRIVATE mini_infer)

add_test(NAME test_tensor COMMAND $<TARGET_FILE:test_tensor>)
add_executable(test_dynamic_array
    m0/tests/test_dynamic_array.cpp
)
target_include_directories(test_dynamic_array PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/m0/include
)
add_test(NAME test_dynamic_array COMMAND $<TARGET_FILE:test_dynamic_array>)

add_executable(test_tensor_catch2
    tests/test_tensor_catch2.cpp
)
target_include_directories(test_tensor_catch2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/catch2
)
target_link_libraries(test_tensor_catch2 PRIVATE mini_infer)

catch_discover_tests(test_tensor_catch2)

add_executable(bench_matmul
    benchmarks/bench_matmul.cpp
)
target_link_libraries(bench_matmul PRIVATE mini_infer)

add_executable(batch_infer
    examples/batch_infer.cpp
)
target_link_libraries(batch_infer PRIVATE mini_infer)

add_executable(test_mlp
    tests/test_mlp.cpp
)
target_include_directories(test_mlp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/catch2
)
target_link_libraries(test_mlp PRIVATE mini_infer)
catch_discover_tests(test_mlp)

add_executable(bench_at
    benchmarks/bench_at.cpp
)
target_link_libraries(bench_at PRIVATE mini_infer)

add_executable(infer_server
    examples/http_server.cpp
)
target_include_directories(infer_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/httplib
)
target_link_libraries(infer_server PRIVATE mini_infer)

if (WIN32)
    target_link_libraries(infer_server PRIVATE ws2_32)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(infer_server PRIVATE Threads::Threads)
endif()