((tasks
  (build
   (:name "All")
   (:window "Build/Run %p")
   (:command "grunt" "cmake_build --current-only"))
  (run
   (:name "Run...")
   (:window "Build/Run %p")
   (:command "grunt" "cmake_build_run --target=ex_loop2"))
  (clean
   (:name "Clean build")
   (:window "Build/Run %p")
   (:command "grunt" "cmake_clean"))
  (cmake
   (:name "Configure CMake")
   (:window "Build/Run %p")
   (:command "grunt" "cmake_config --current-only"))
  (cmake-clear-cache
   (:name "Clear Cache and Configure CMake")
   (:window "Build/Run %p")
   (:command "grunt" "cmake_config --current-only --clear-cache"))
  (config-clang-debug
   (:name "Set config: clang-Debug")
   (:window "Build/Run %p")
   (:command "grunt" "cmake_set --current-config=clang-Debug"))
  (config-clang-sanitize
   (:name "Set config: clang-Sanitize")
   (:window "Build/Run %p")
   (:command "grunt" "cmake_set --current-config=clang-Sanitize"))
  (config-clang-release
   (:name "Set config: clang-Release")
   (:window "Build/Run %p")
   (:command "grunt" "cmake_set --current-config=clang-Release"))
  (config-gcc-debug
   (:name "Set config: gcc-Debug")
   (:window "Build/Run %p")
   (:command "grunt" "cmake_set --current-config=gcc-Debug"))
  (config-gcc-sanitize
   (:name "Set config: gcc-Sanitize")
   (:window "Build/Run %p")
   (:command "grunt" "cmake_set --current-config=gcc-Sanitize"))
  (config-gcc-release
   (:name "Set config: gcc-Release")
   (:window "Build/Run %p")
   (:command "grunt" "cmake_set --current-config=gcc-Release"))
  ))
