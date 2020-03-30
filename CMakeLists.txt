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
set(CMAKE_CXX_STANDARD 17)

function(FixUsrInclude TARGET)
  get_target_property(_includes ${TARGET} INTERFACE_INCLUDE_DIRECTORIES)
  list(REMOVE_ITEM _includes "/usr/include")
  set_target_properties(${TARGET} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${_includes}")

  get_target_property(_deps "${TARGET}" INTERFACE_LINK_LIBRARIES)

  if(NOT "${_deps}" STREQUAL "_deps-NOTFOUND")
    foreach(_dep ${_deps})
      FixUsrInclude(${_dep})
    endforeach()
  endif()
endfunction()

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)

FixUsrInclude(OpenGL::GL)
FixUsrInclude(GLEW::GLEW)

find_package(GameKernelCore REQUIRED)
find_package(rpav_cxxutils REQUIRED)
find_package(nanovg REQUIRED)
find_package(nlohmann_json REQUIRED)

if(GK_USE_BOX2D)
  find_package(Box2D)
  add_definitions(-DGK_USE_BOX2D=1)
  list(APPEND GK_OPTIONAL_DEPS
    PRIVATE Box2D)
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
