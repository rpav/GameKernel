cmake_minimum_required(VERSION 3.2)

project(gk_examples)

if(WIN32)
  set(GL opengl32)
else()
  set(GL GL)
endif()

add_executable(example1 example1.cpp main.cpp)
target_link_libraries(example1 SDL2 ${GL} gk-static)

if(GK_BUILD_GKPP)
  add_executable(example1a example1a.cpp main.cpp)
  target_link_libraries(example1a SDL2 ${GL} gk-static gk++-static)
endif()

add_executable(example2 example2.cpp main.cpp)
target_link_libraries(example2 SDL2 ${GL} gk-shared)

add_executable(example3 example3.cpp main.cpp)
target_link_libraries(example3 SDL2 ${GL} gk-static)

if(GK_BUILD_GKPP)
  add_executable(example4 example4.cpp main.cpp)
  target_link_libraries(example4 SDL2 ${GL} gk-static gk++-static)

  add_executable(example5 example5.cpp main.cpp)
  target_link_libraries(example5 SDL2 ${GL} gk-static gk++-static)

  add_executable(example6 example6.cpp main.cpp)
  target_link_libraries(example6 SDL2 ${GL} gk-static gk++-static)
endif()
