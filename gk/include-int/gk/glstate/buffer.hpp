#pragma once

namespace gk {
    // gk::GLStateBuffer
    class GLStateBuffer : public GLState {
        BufferTarget _target;
        GLuint _name;

    public:
        GLStateBuffer(BufferTarget target, GLuint name = 0)
            : _target(target), _name(name) { }

        void set(GLuint name, GLStateConfig &c) {
            if(_name != name) {
                _name = name;
                c.dirty = true;
            }
        }

        virtual void apply(GLGlobalState &g) {
            g.bindBuffer(_target, _name);
        }
    };
}
