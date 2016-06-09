#pragma once

#include <vector>

namespace gk {
    typedef std::vector<GLuint> ShaderVector;

    struct GLProgram {
        GLuint program;
        UniformSet uniforms;

        GLProgram(GLuint program_ = 0) : program(program_) { }

        inline GLuint findUniform(const char *name) {
            return glGetUniformLocation(program, name);
        }

        inline operator GLuint() { return program; }
        
        virtual void postLink() { }
    };

    class GLProgramBuilder {
        ShaderVector _shaders;
    public:
        GLProgramBuilder() {
            _shaders.reserve(3);
        }

        template<typename...Rest>
        inline void add(GLenum type, const char *src, Rest...args) {
            auto shader = gk_gl_compile_shader(type, src);
            _shaders.push_back(shader);

            add(args...);
        }

        inline void add() { }

        inline bool link(GLProgram &program) {
            program.program = gk_gl_link_program(_shaders.size(), _shaders.data());

            if(program.program) {
                program.postLink();
                return true;
            }
            return false;
        }
    };
}
