#pragma once

namespace gk {
    
    // gk::GLStateProgram
    class GLStateProgram : public GLState {
        GLuint _program;
    public:
        inline virtual void apply(GLGlobalState &g) {
            g.useProgram(_program);
        }

        inline bool set(GLuint program, GLStateConfig &c) {
            if(_program != program) {
                _program = program;
                c.dirty = true;
                return true;
            }
            return false;
        }
    };
}
