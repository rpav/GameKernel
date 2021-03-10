cmake_minimum_required(VERSION 3.2)

set(GK_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE GK_SOURCE_NVG
  ${GK_ROOT}/nvg.cpp
  )

if(GK_USE_BOX2D)
  add_definitions(-DGK_USE_BOX2D)
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
  )

set(GK_HEADER_FILES
  ${GK_HEADER_CORE}
  ${GK_INT_HEADER_GLOB}
  ${GK_EXT_HEADER_GLOB}
  )

if(GK_BUILD_SHARED)
  add_library(gk-shared SHARED ${GK_SOURCE_FILES} ${GK_HEADER_FILES})
  set_target_properties(gk-shared PROPERTIES OUTPUT_NAME gk)
  target_include_directories(gk-shared
    PUBLIC ${PROJ_ROOT}/gk/include
    PRIVATE ${PROJ_ROOT}/gk/include-int
    PRIVATE ${PROJ_ROOT}/gk++/include
    )
  target_cxx_std(gk-shared 17)
  target_link_libraries(gk-shared
    PUBLIC rpav_cxxutils
    PUBLIC GameKernelCore
    PUBLIC OpenGL::GL
    PRIVATE GLEW
    PRIVATE nlohmann_json
    PRIVATE nanovg
    ${GK_OPTIONAL_DEPS}
    )

  if(WIN32)
    add_definitions(-DGK_EXPORT)
  endif()

  if(GK_INSTALL)
    if(WIN32)
      install(TARGETS gk-shared RUNTIME DESTINATION lib/)
    else()
      install(TARGETS gk-shared LIBRARY DESTINATION lib/)
    endif()
  endif()
endif()

if(GK_BUILD_STATIC)
  add_library(gk-static STATIC ${GK_SOURCE_FILES} ${GK_HEADER_FILES})
  set_target_properties(gk-static PROPERTIES OUTPUT_NAME gk)
  target_include_directories(gk-static
    PUBLIC ${PROJ_ROOT}/gk/include
    PRIVATE ${PROJ_ROOT}/gk/include-int
    PRIVATE ${PROJ_ROOT}/gk++/include
    )
  target_cxx_std(gk-static 17)
  target_link_libraries(gk-static
    PUBLIC rpav_cxxutils
    PUBLIC GameKernelCore
    PUBLIC OpenGL::GL
    PRIVATE GLEW
    PRIVATE nlohmann_json
    PRIVATE nanovg
    ${GK_OPTIONAL_DEPS}
    )

  if(GK_INSTALL)
    install(TARGETS gk-static ARCHIVE DESTINATION lib/)
  endif()
endif()

if(GK_INSTALL)
  install(
    DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN *.h
    )
endif()
