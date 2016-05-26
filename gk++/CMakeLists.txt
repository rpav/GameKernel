cmake_minimum_required(VERSION 3.2)

### Basic config ##########################################################

set(LIB_NAME gk++)
project(${LIB_NAME})

set(GKPP_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

### Modules ###############################################################

file(GLOB_RECURSE GKPP_SOURCE_GLOB
  ${GKPP_ROOT}/*.cpp
  )

file(GLOB GKPP_HEADER_GLOB
  ${GKPP_ROOT}/include/gk/*.hpp
  )

set(GKPP_SOURCE_FILES
  ${GKPP_SOURCE_GLOB}
  )

set(GKPP_HEADER_FILES
  ${GKPP_HEADER_GLOB}
  )

if(GK_BUILD_SHARED)
  add_library(${LIB_NAME}-shared SHARED ${GKPP_SOURCE_FILES})
  set_target_properties(${LIB_NAME}-shared PROPERTIES OUTPUT_NAME ${LIB_NAME})
  target_link_libraries(${LIB_NAME}-shared ${GK_LIBS})

  if(GK_INSTALL)
    install(TARGETS ${LIB_NAME}-shared LIBRARY DESTINATION lib/)
  endif()
endif()

if(GK_BUILD_STATIC)
  add_library(${LIB_NAME}-static STATIC ${GKPP_SOURCE_FILES})
  set_target_properties(${LIB_NAME}-static PROPERTIES OUTPUT_NAME ${LIB_NAME})
  target_link_libraries(${LIB_NAME}-static ${GK_LIBS})

  if(GK_INSTALL)
    install(TARGETS ${LIB_NAME}-static ARCHIVE DESTINATION lib/)
  endif()
endif()

if(GK_INSTALL)
  install(
    DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN *.hpp
    )
endif()
