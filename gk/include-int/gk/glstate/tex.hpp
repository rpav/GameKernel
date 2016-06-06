#pragma once

namespace gk {
    // gk::GLStateTex
    class GLStateTex : public GLState {
        GLuint _unit;            // GL_TEXTURE0 + _unit
        GLenum _target;
        GLuint _tex;

    public:
        GLStateTex(GLuint unit, GLenum target, GLuint tex = 0)
            : _unit(unit), _target(target), _tex(tex) { }

        inline bool set(GLuint tex, GLStateConfig &c) {
            if(_tex != tex) {
                _tex = tex;
                c.dirty = true;
                return true;
            }
            return false;
        }

        inline virtual void apply(GLGlobalState &g) {
            g.bindTexture(_unit, _target, _tex);
        }
    };
}
