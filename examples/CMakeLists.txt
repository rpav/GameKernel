cmake_minimum_required(VERSION 3.2)

project(gk_examples)

if(WIN32)
  set(GL opengl32)
else()
  set(GL GL)
endif()

add_executable(gk_example1 example1.cpp main.cpp)
target_link_libraries(gk_example1
  PRIVATE rpav_cxxutils::rpav_cxxutils
  PRIVATE SDL2
  PRIVATE ${GL}
  PRIVATE gk-static)

if(GK_BUILD_GKPP)
  add_executable(gk_example1a example1a.cpp main.cpp)
  target_link_libraries(gk_example1a
    PRIVATE rpav_cxxutils::rpav_cxxutils
    PRIVATE SDL2
    PRIVATE ${GL}
    PRIVATE gk++-static)
endif()

add_executable(gk_example2 example2.cpp main.cpp)
target_link_libraries(gk_example2
  PRIVATE rpav_cxxutils::rpav_cxxutils
  PRIVATE SDL2
  PRIVATE ${GL}
  PRIVATE gk-static)

add_executable(gk_example3 example3.cpp main.cpp)
target_link_libraries(gk_example3
  PRIVATE rpav_cxxutils::rpav_cxxutils
  PRIVATE SDL2
  PRIVATE ${GL}
  PRIVATE gk-static)

if(GK_BUILD_GKPP)
  add_executable(gk_example4 example4.cpp main.cpp)
  target_link_libraries(gk_example4
    PRIVATE rpav_cxxutils::rpav_cxxutils
    PRIVATE SDL2
    PRIVATE ${GL}
    PRIVATE gk++-static)

  add_executable(gk_example5 example5.cpp main.cpp)
  target_link_libraries(gk_example5
    PRIVATE rpav_cxxutils::rpav_cxxutils
    PRIVATE SDL2
    PRIVATE ${GL}
    PRIVATE gk++-static)

  add_executable(gk_example6 example6.cpp main.cpp)
  target_link_libraries(gk_example6
    PRIVATE rpav_cxxutils::rpav_cxxutils
    PRIVATE SDL2
    PRIVATE ${GL}
    PRIVATE gk++-static)
endif()
