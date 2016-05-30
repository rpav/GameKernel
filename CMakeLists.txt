cmake_minimum_required(VERSION 3.2)

# Build options
option(GK_INSTALL "Install GK (or not)" ON)
option(GK_BUILD_STATIC "Build static library" ON)
option(GK_BUILD_SHARED "Build shared library" ON)
option(GK_BUILD_GKPP "Build GK++ library (required for some examples)" ON)

# Optional functionality
option(GK_USE_BOX2D "Compile in Box2D support" ON)
option(GK_USE_TEXTUREPACKER "Compile in generic TexturePacker JSON support" ON)

## Everything
set(PROJ_ROOT ${CMAKE_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(GLEW REQUIRED)
if(GLEW_FOUND)
  include_directories(${GLEW_INCLUDE_DIRS})
  link_libraries(${GLEW_LIBRARIES})
endif()

if(GK_USE_BOX2D)
  set(BOX2D_BUILD_EXAMPLES OFF CACHE BOOL "turn off Box2D examples")
  set(BOX2D_BUILD_SHARED ON CACHE BOOL "turn off Box2D shared")
  set(BOX2D_BUILD_STATIC ON CACHE BOOL "turn on Box2D static")
  set(BOX2D_INSTALL OFF CACHE BOOL "don't install Box2D")
  add_subdirectory(${PROJ_ROOT}/third-party/Box2D/Box2D)
  add_definitions(-DGK_USE_BOX2D=1)

  include_directories(
    ${PROJ_ROOT}/third-party/Box2D/Box2D
    )
  set(GK_LIBS_STATIC ${GK_LIBS_STATIC} Box2D)
  set(GK_LIBS_SHARED ${GK_LIBS_SHARED} Box2D_shared)
endif()

if(GK_USE_TEXTUREPACKER)
  add_definitions(-DGK_USE_TEXTUREPACKER=1)
endif()

include_directories(
  ${PROJ_ROOT}/third-party/nanovg/src
  ${PROJ_ROOT}/third-party/json/src
  ${PROJ_ROOT}/gk/include
  ${PROJ_ROOT}/gk/include-int
  ${PROJ_ROOT}/gk++/include
  )

add_subdirectory(gk)

if(GK_BUILD_GKPP)
  add_subdirectory(gk++)
endif()

add_subdirectory(examples)
