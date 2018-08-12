cmake_minimum_required(VERSION 3.2)

### Basic config ##########################################################

set(LIB_NAME gk)
project(${LIB_NAME})

set(GK_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

### Modules ###############################################################

file(GLOB_RECURSE GK_SOURCE_NVG
  ${GK_ROOT}/nvg.cpp
  )

if(GK_USE_BOX2D)
  file(GLOB_RECURSE GK_SOURCE_BOX2D
    ${GK_ROOT}/box2d.cpp
    ${GK_ROOT}/box2d/*.cpp
    )
endif()

file(GLOB_RECURSE GK_SOURCE_GL
  ${GK_ROOT}/gl.cpp
  ${GK_ROOT}/gl_state.cpp
  ${GK_ROOT}/gl2/*.cpp
  ${GK_ROOT}/gl3/*.cpp
  )

file(GLOB_RECURSE GK_SOURCE_SPRITESHEET
  ${GK_ROOT}/spritesheet.cpp
  ${GK_ROOT}/spritesheet/*.cpp
  )

file(GLOB GK_HEADER_CORE
  ${PROJ_ROOT}/core/include/gk/*.h
  )

file(GLOB_RECURSE GK_INT_HEADER_GLOB
  ${GK_ROOT}/include-int/**/*.hpp
  ${GK_ROOT}/include-int/**/*.h
  )

file(GLOB_RECURSE GK_EXT_HEADER_GLOB
  ${GK_ROOT}/include/**/*.h
)

set(GK_SOURCE_FILES
  c-api.cpp
  errors.cpp
  gk.cpp
  transform.cpp

  ${GK_SOURCE_NVG}
  ${GK_SOURCE_GL}
  ${GK_SOURCE_SPRITESHEET}
  ${GK_SOURCE_BOX2D}

  ${PROJ_ROOT}/third-party/nanovg/src/nanovg.c
  )

set(GK_HEADER_FILES
  ${GK_HEADER_CORE}
  ${GK_INT_HEADER_GLOB}
  ${GK_EXT_HEADER_GLOB}
  )

if(GK_BUILD_SHARED)
  add_library(${LIB_NAME}-shared SHARED ${GK_SOURCE_FILES} ${GK_HEADER_FILES})
  set_target_properties(${LIB_NAME}-shared PROPERTIES OUTPUT_NAME ${LIB_NAME})
  target_include_directories(${LIB_NAME}-shared
    PUBLIC ${PROJ_ROOT}/gk/include
    )
  target_link_libraries(${LIB_NAME}-shared
    PUBLIC rpav::cxxutils
    PUBLIC rpav::GameKernelCore
    PUBLIC ${GL}
    PRIVATE Box2D
    )

  if(WIN32)
    add_definitions(-DGK_EXPORT)
  endif()

  if(GK_INSTALL)
    if(WIN32)
      install(TARGETS ${LIB_NAME}-shared RUNTIME DESTINATION lib/)
    else()
      install(TARGETS ${LIB_NAME}-shared LIBRARY DESTINATION lib/)
    endif()
  endif()
endif()

if(GK_BUILD_STATIC)
  add_library(${LIB_NAME}-static STATIC ${GK_SOURCE_FILES} ${GK_HEADER_FILES})
  set_target_properties(${LIB_NAME}-static PROPERTIES OUTPUT_NAME ${LIB_NAME})
  target_include_directories(${LIB_NAME}-static
    PUBLIC ${PROJ_ROOT}/gk/include
    )
  target_link_libraries(${LIB_NAME}-static
    PUBLIC rpav::cxxutils
    PUBLIC rpav::GameKernelCore
    PUBLIC ${GL}
    PRIVATE Box2D
    )

  if(NOT WIN32)
    cotire(${LIB_NAME}-static)
  endif()

  if(GK_INSTALL)
    install(TARGETS ${LIB_NAME}-static ARCHIVE DESTINATION lib/)
  endif()
endif()

if(GK_INSTALL)
  install(
    DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN *.h
    )
endif()
