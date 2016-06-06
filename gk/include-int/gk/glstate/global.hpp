#pragma once

#include <map>
#include <vector>

namespace gk {
    typedef std::pair<GLuint, GLuint> UnitTargetBinding;
    typedef std::map<UnitTargetBinding, GLuint> TextureMap;

    // This tracks all "global state"
    class GLGlobalState {
        GLuint active_program;
        GLenum active_texture;

        TextureMap textures;

    public:
        GLuint vertex_array;
        GLuint array_buffer;

        void reset();

        inline bool activeTexture(GLuint unit) {
            if(active_texture != unit) {
                glActiveTexture(GL_TEXTURE0 + unit);
                active_texture = unit;
                return true;
            }
            return false;
        }

        inline GLuint getBoundTexture(GLuint unit, GLuint target) {
            auto k = UnitTargetBinding(unit, target);
            auto v = textures.find(k);

            if(v != textures.end())
                return v->second;
            else
                return 0;
        }

        inline bool bindTexture(GLuint unit, GLuint target, GLuint tex) {
            if(getBoundTexture(unit, target) != tex) {
                auto k = UnitTargetBinding(unit, target);

                activeTexture(unit);

                glBindTexture(target, tex);
                textures[k] = tex;

                return true;
            }
            return false;
        }

        inline bool useProgram(GLuint program) {
            if(active_program != program) {
                glUseProgram(program);
                active_program = program;
                return true;
            }
            return false;
        }
    };
}
