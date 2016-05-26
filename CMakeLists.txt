cmake_minimum_required(VERSION 3.2)

### Basic config ##########################################################

set(LIB_NAME gk)
project(${LIB_NAME})

set(SOURCE_ROOT ${CMAKE_SOURCE_DIR})

# Build options
option(GK_INSTALL "Install GK (or not)" ON)
option(GK_BUILD_STATIC "Build static library" ON)
option(GK_BUILD_SHARED "Build shared library" ON)

# Optional functionality
option(GK_USE_BOX2D "Compile in Box2D support" OFF)
option(GK_USE_TEXTUREPACKER "Compile in generic TexturePacker JSON support" ON)

### Modules ###############################################################

find_package(GLEW REQUIRED)
if(GLEW_FOUND)
  include_directories(${GLEW_INCLUDE_DIRS})
  link_libraries(${GLEW_LIBRARIES})
endif()

if(GK_USE_BOX2D)
  set(BOX2D_BUILD_EXAMPLES OFF CACHE BOOL "turn off Box2D examples")
  set(BOX2D_BUILD_SHARED OFF CACHE BOOL "turn off Box2D shared")
  set(BOX2D_BUILD_STATIC ON CACHE BOOL "turn on Box2D static")
  set(BOX2D_INSTALL OFF CACHE BOOL "don't install Box2D")
  add_subdirectory(third-party/Box2D/Box2D)
  add_definitions(-DGK_USE_BOX2D=1)

  include_directories(
    ${SOURCE_ROOT}/third-party/Box2D/Box2D
    )
  set(GK_LIBS ${GK_LIBS} Box2D)
endif()

if(GK_USE_TEXTUREPACKER)
  add_definitions(-DGK_USE_TEXTUREPACKER=1)
endif()

### Variables #############################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

### Source ################################################################

include_directories(
  ${SOURCE_ROOT}/include
  ${SOURCE_ROOT}/third-party/nanovg/src
  ${SOURCE_ROOT}/third-party/json/src
  )

file(GLOB_RECURSE GK_SOURCE_GLOB
  ${SOURCE_ROOT}/src/*.cpp
  ${SOURCE_ROOT}/src/*.c
  )

file(GLOB GK_HEADER_GLOB
  ${SOURCE_ROOT}/include/gk/*.hpp
  ${SOURCE_ROOT}/include/gk/*.h
  )

set(GK_SOURCE_FILES
  ${GK_SOURCE_GLOB}

  ${SOURCE_ROOT}/third-party/nanovg/src/nanovg.c
  )

set(GK_HEADER_FILES
  ${GK_HEADER_GLOB}
  )

if(GK_BUILD_SHARED)
  add_library(${LIB_NAME}_shared SHARED ${GK_SOURCE_FILES})
  set_target_properties(${LIB_NAME}_shared PROPERTIES OUTPUT_NAME ${LIB_NAME})
  target_link_libraries(${LIB_NAME}_shared ${GK_LIBS})

  if(GK_INSTALL)
    install(TARGETS ${LIB_NAME}_shared LIBRARY DESTINATION lib/)
  endif()
endif()

if(GK_BUILD_STATIC)
  add_library(${LIB_NAME}_static STATIC ${GK_SOURCE_FILES})
  set_target_properties(${LIB_NAME}_static PROPERTIES OUTPUT_NAME ${LIB_NAME})
  target_link_libraries(${LIB_NAME}_static ${GK_LIBS})

  if(GK_INSTALL)
    install(TARGETS ${LIB_NAME}_static ARCHIVE DESTINATION lib/)
  endif()
endif()

if(GK_INSTALL)
  install(
    DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN *.h
    )
endif()

add_subdirectory(examples)

### Custom Targets ########################################################

# add_custom_target(name
#   COMMAND ...
#   )
