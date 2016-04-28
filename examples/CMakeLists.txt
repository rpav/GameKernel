cmake_minimum_required(VERSION 3.2)

project(gk_examples)

add_executable(example1 example1.cpp main.cpp)
target_link_libraries(example1 SDL2 GL gk_static)

add_executable(example2 example2.cpp main.cpp)
target_link_libraries(example2 SDL2 GL gk_static)

add_executable(example3 example3.cpp main.cpp)
target_link_libraries(example3 SDL2 GL gk_static)
