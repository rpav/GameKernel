cmake_minimum_required(VERSION 3.2)

# Build options
option(GK_INSTALL "Install GK (or not)" ON)
option(GK_BUILD_STATIC "Build static library" ON)
option(GK_BUILD_SHARED "Build shared library" ON)
option(GK_BUILD_GKPP "Build GK++ library (required for some examples)" ON)
option(GK_BUILD_EXAMPLES "Build examples" ON)

# Optional functionality
option(GK_USE_BOX2D "Compile in Box2D support" ON)
option(GK_USE_TEXTUREPACKER "Compile in generic TexturePacker JSON support" ON)

## Everything
set(PROJ_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT WIN32)
  include(cotire)
endif()

find_package(GLEW REQUIRED)
if(GLEW_FOUND)
  include_directories(${GLEW_INCLUDE_DIRS})
  link_libraries(${GLEW_LIBRARIES})
  #add_definitions(-DGLEW_STATIC)
endif()

if(WIN32)
  set(GK_GL_LIB opengl32.lib)
else()
  set(GK_GL_LIB GL)
endif()

if(GK_USE_BOX2D)
  set(BOX2D_BUILD_EXAMPLES OFF CACHE BOOL "turn off Box2D examples")
  set(BOX2D_BUILD_SHARED OFF CACHE BOOL "turn off Box2D shared")
  set(BOX2D_BUILD_STATIC ON CACHE BOOL "turn on Box2D static")
  set(BOX2D_INSTALL OFF CACHE BOOL "don't install Box2D")
  add_subdirectory(${PROJ_ROOT}/third-party/Box2D/Box2D)
  add_definitions(-DGK_USE_BOX2D=1)

  include_directories(
    ${PROJ_ROOT}/third-party/Box2D/Box2D
    )
endif()

if(GK_USE_TEXTUREPACKER)
  add_definitions(-DGK_USE_TEXTUREPACKER=1)
endif()

include_directories(
  ${PROJ_ROOT}/third-party/nanovg/src
  ${PROJ_ROOT}/third-party/json
  ${PROJ_ROOT}/gk/include
  ${PROJ_ROOT}/gk/include-int
  ${PROJ_ROOT}/gk++/include
  )

add_subdirectory(gk)

if(GK_BUILD_GKPP)
  add_subdirectory(gk++)
endif()

if(GK_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
