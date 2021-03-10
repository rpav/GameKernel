const { cmake } = require('./CMake/gruntutil.js');

module.exports = function(grunt) {
  grunt.loadNpmTasks('grunt-rpav-cmake');
  grunt.loadNpmTasks('grunt-run');

  grunt.initConfig({
    cmake_config: {
      options: {
        buildDir: o => "build/" + o.buildConfig,
        env: {
          LSAN_OPTIONS: "suppressions=$PROJECT_ROOT/lsan_suppress.txt",
        },
      },

      configs: {
        "clang-Debug": cmake({ tc: 'clang', c: 'Debug' }),
        "clang-Sanitize": cmake({ tc: 'clang', c: 'Sanitize' }),
        "clang-Release": cmake({ tc: 'clang', c: 'Release' }),

        "gcc-Debug": cmake({ tc: 'gcc', c: 'Debug' }),
        "gcc-Sanitize": cmake({ tc: 'gcc', c: 'Sanitize' }),
        "gcc-Release": cmake({ tc: 'gcc', c: 'Release' }),
      },
    },

    cmake_run: {
      all: { build: "all" },
      example: { build: "", run: "./example", cwd: "bin" },
    },

    cmake_build_run: {
      example: {},
    },

    run: {},
  });
};
