#pragma once

namespace gk {
    // gk::GLStateVao
    class GLStateVao : public GLState {
        GLuint _vao;
    public:
        GLStateVao(GLuint vao = 0)
            : _vao(vao) { }

        void set(GLuint vao, GLStateConfig &c) {
            if(_vao != vao) {
                _vao = vao;
                c.dirty = true;
            }
        }

        inline virtual void apply(GLGlobalState &g) {
            g.bindVertexArray(_vao);
        }

        inline virtual void clean(GLGlobalState &g) {
            g.deleteVertexArrays(1, &_vao);
        }
    };
}
