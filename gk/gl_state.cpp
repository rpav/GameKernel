#include <string.h>

#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/log.hpp"

#define CHECK_SET_STATE(state, name, newval)    \
    do {                                        \
        if((newval) == state.name)              \
            return;                             \
        else                                    \
            state.name = (newval);              \
    } while(0);

void gk_gl_reset_state(gk_context *gk) {
    auto &state = gk->impl_data->glstate;
    state.reset();
}

void gk::GLGlobalState::reset() {
    active_program = 0;
    active_texture = 0;
    vertex_array = 0;
    array_buffer = 0;
    textures.clear();
}

void gk_glBindVertexArray(gk_context *gk, GLuint array) {
    auto &state = gk->impl_data->glstate;

    CHECK_SET_STATE(state, vertex_array, array);
    glBindVertexArray(array);
}

void gk_glBindBuffer(gk_context *gk, GLenum target, GLuint buffer) {
    auto &state = gk->impl_data->glstate;

    switch(target) {
        case GL_ARRAY_BUFFER:
            CHECK_SET_STATE(state, array_buffer, buffer);
            break;
    }
    glBindBuffer(target, buffer);
}
