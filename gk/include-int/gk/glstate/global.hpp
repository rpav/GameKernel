#pragma once

#include <map>
#include <vector>

#include <rpav/log.hpp>

#include "gk/glutil.hpp"

using namespace rpav;

namespace gk {
enum BufferTarget {
    BUFFER_ARRAY,
    BUFFER_ATOMIC_COUNTER,
    BUFFER_COPY_READ,
    BUFFER_COPY_WRITE,
    BUFFER_DISPATCH_INDIRECT,
    BUFFER_DRAW_INDIRECT,
    BUFFER_ELEMENT_ARRAY,
    BUFFER_PIXEL_PACK,
    BUFFER_PIXEL_UNPACK,
    BUFFER_QUERY,
    BUFFER_SHADER_STORAGE,
    BUFFER_TEXTURE,
    BUFFER_TRANSFORM_FEEDBACK,
    BUFFER_UNIFORM,

    BUFFER_MAX
};
extern const GLenum target_to_gl[];

typedef std::pair<GLuint, GLuint>           UnitTargetBinding;
typedef std::map<UnitTargetBinding, GLuint> TextureMap;
typedef std::vector<GLuint>                 BufferBinding;

// This tracks all "global state"
class GLGlobalState {
    GLuint active_program;
    GLenum active_texture;

    TextureMap    textures;
    BufferBinding buffers;

    GLuint bound_vertex_array;

public:
    GLGlobalState() : buffers(BUFFER_MAX, 0) { reset(); }

    void reset();

    inline bool activeTexture(GLuint unit)
    {
        if(active_texture != unit) {
            GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
            active_texture = unit;
            return true;
        }
    gl_error:
        return false;
    }

    inline GLuint getBoundTexture(GLuint unit, GLuint target)
    {
        auto k = UnitTargetBinding(unit, target);
        auto v = textures.find(k);

        if(v != textures.end())
            return v->second;
        else
            return 0;
    }

    inline bool bindTexture(GLuint unit, GLuint target, GLuint tex)
    {
        if(getBoundTexture(unit, target) != tex) {
            auto k = UnitTargetBinding(unit, target);

            activeTexture(unit);

            GL_CHECK(glBindTexture(target, tex));
            textures[k] = tex;

            return true;
        }
    gl_error:
        return false;
    }

    inline bool useProgram(GLuint program)
    {
        if(active_program != program) {
            GL_CHECK(glUseProgram(program));
            active_program = program;
            return true;
        }
    gl_error:
        return false;
    }

    inline bool bindBuffer(BufferTarget target, GLuint buffer)
    {
        if(buffers[target] != buffer) {
            GL_CHECK(glBindBuffer(target_to_gl[target], buffer));
            buffers[target] = buffer;
            return true;
        }
    gl_error:
        return false;
    }

    inline bool bindVertexArray(GLuint vao)
    {
        if(bound_vertex_array != vao) {
            GL_CHECK(glBindVertexArray(vao));
            bound_vertex_array = vao;
            return true;
        }
    gl_error:
        return false;
    }

    inline void deleteVertexArrays(GLsizei n, const GLuint* arrays)
    {
        for(GLsizei i = 0; i < n; ++i) {
            if(arrays[i] == bound_vertex_array) bound_vertex_array = 0;
        }

        GL_CHECK(glDeleteVertexArrays(n, arrays));
    gl_error:
        return;
    }
};
} // namespace gk
