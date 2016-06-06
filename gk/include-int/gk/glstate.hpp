#pragma once

#include <map>
#include <vector>

namespace gk {
    class GLGlobalState;

    // gk::GLState
    class GLState {
    public:
        virtual ~GLState() {}
        virtual void apply(GLGlobalState&) = 0;
    };
}

#include "gk/glstate/global.hpp"
#include "gk/glstate/config.hpp"
#include "gk/glstate/tex.hpp"
#include "gk/glstate/program.hpp"
#include "gk/glstate/buffer.hpp"
#include "gk/glstate/vao.hpp"

struct gk_context;

// State-tracking API
void gk_gl_reset_state(gk_context *gk);

void gk_glBindVertexArray(gk_context *gk, GLuint array);
