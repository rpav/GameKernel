cmake_minimum_required(VERSION 3.2)

### Basic config ##########################################################

set(LIB_NAME gk)
project(${LIB_NAME})

set(GK_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

### Modules ###############################################################

file(GLOB_RECURSE GK_SOURCE_GLOB
  ${GK_ROOT}/*.cpp
  ${GK_ROOT}/*.c
  )

file(GLOB GK_HEADER_GLOB
  ${GK_ROOT}/include/gk/*.hpp
  ${GK_ROOT}/include/gk/*.h
  )

set(GK_SOURCE_FILES
  ${GK_SOURCE_GLOB}

  ${PROJ_ROOT}/third-party/nanovg/src/nanovg.c
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
