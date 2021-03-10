include(CPM)

CPMAddPackage(
  NAME CMakeSetup
  GIT_TAG master
  GITHUB_REPOSITORY rpav/CMakeSetup
  DOWNLOAD_ONLY
  )
list(APPEND CMAKE_MODULE_PATH "${CMakeSetup_SOURCE_DIR}")

include(setup)
