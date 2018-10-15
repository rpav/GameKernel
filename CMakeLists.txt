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
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT WIN32)
  include(cotire)
endif()

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

if(GK_USE_BOX2D)
  add_definitions(-DGK_USE_BOX2D=1)
  list(APPEND GK_OPTIONAL_DEPS
    PRIVATE erincatto::Box2D)
endif()

if(GK_USE_TEXTUREPACKER)
  add_definitions(-DGK_USE_TEXTUREPACKER=1)
endif()

add_subdirectory(gk)

if(GK_BUILD_GKPP)
  add_subdirectory(gk++)
endif()

if(GK_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
