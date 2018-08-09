cmake_minimum_required(VERSION 3.2)

### Basic config ##########################################################

set(LIB_NAME gk++)
project(${LIB_NAME})

set(GKPP_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

### Modules ###############################################################

file(GLOB_RECURSE GKPP_SOURCE_GLOB
  ${GKPP_ROOT}/*.cpp
  )

file(GLOB_RECURSE GKPP_HEADER_GLOB
  ${GKPP_ROOT}/**/*.hpp
  )

set(GKPP_SOURCE_FILES
  ${GKPP_SOURCE_GLOB}
  )

file(GLOB GKPP_HEADER_CORE
  ${PROJ_ROOT}/core/include/gk/*.hpp
  )

set(GKPP_HEADER_FILES
  ${GKPP_HEADER_CORE}
  ${GKPP_HEADER_GLOB}
  )

source_group("Header Files\\Core" FILES ${GKPP_HEADER_CORE})

if(GK_BUILD_SHARED)
  add_library(${LIB_NAME}-shared SHARED ${GKPP_SOURCE_FILES} ${GKPP_HEADER_FILES})
  set_target_properties(${LIB_NAME}-shared PROPERTIES OUTPUT_NAME ${LIB_NAME})
  target_include_directories(${LIB_NAME}-shared
    PUBLIC ${PROJ_ROOT}/gk++/include
    )
  target_link_libraries(${LIB_NAME}-shared
    PUBLIC gk-shared
    )

  if(GK_INSTALL)
    if(WIN32)
      install(TARGETS ${LIB_NAME}-shared RUNTIME DESTINATION lib/)
    else()
      install(TARGETS ${LIB_NAME}-shared LIBRARY DESTINATION lib/)
    endif()
  endif()
endif()

if(GK_BUILD_STATIC)
  add_library(${LIB_NAME}-static STATIC ${GKPP_SOURCE_FILES} ${GKPP_HEADER_FILES})
  set_target_properties(${LIB_NAME}-static PROPERTIES OUTPUT_NAME ${LIB_NAME})
  target_include_directories(${LIB_NAME}-static
    PUBLIC ${PROJ_ROOT}/gk++/include
    )
  target_link_libraries(${LIB_NAME}-static
    PUBLIC gk-static
    )

  if(NOT WIN32)
    #cotire(${LIB_NAME}-static)
  endif()

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
