cmake_minimum_required(VERSION 3.2)

set(GKPP_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

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
  add_library(gk++-shared SHARED ${GKPP_SOURCE_FILES} ${GKPP_HEADER_FILES})
  set_target_properties(gk++-shared PROPERTIES OUTPUT_NAME gk++)
  target_include_directories(gk++-shared
    PUBLIC ${PROJ_ROOT}/gk++/include
    )
  target_cxx_std(gk++-shared 17)
  target_link_libraries(gk++-shared
    PUBLIC CONAN_PKG::rpav_cxxutils
    PUBLIC gk-shared
    )

  if(GK_INSTALL)
    if(WIN32)
      install(TARGETS gk++-shared RUNTIME DESTINATION lib/)
    else()
      install(TARGETS gk++-shared LIBRARY DESTINATION lib/)
    endif()
  endif()
endif()

if(GK_BUILD_STATIC)
  add_library(gk++-static STATIC ${GKPP_SOURCE_FILES} ${GKPP_HEADER_FILES})
  set_target_properties(gk++-static PROPERTIES OUTPUT_NAME gk++)
  target_include_directories(gk++-static
    PUBLIC ${PROJ_ROOT}/gk++/include
    )
  target_cxx_std(gk++-static 17)
  target_link_libraries(gk++-static
    PUBLIC CONAN_PKG::rpav_cxxutils
    PUBLIC gk-static
    )

  if(NOT WIN32)
    #cotire(gk++-static)
  endif()

  if(GK_INSTALL)
    install(TARGETS gk++-static ARCHIVE DESTINATION lib/)
  endif()
endif()

if(GK_INSTALL)
  install(
    DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN *.hpp
    )
endif()
