#include <string.h>

#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/log.hpp"

#define GLSTATE(gk) ((gk)->impl_data->glstate)

#define CHECK_SET_STATE(state, name, newval)    \
    do {                                        \
        if((newval) == state.name)              \
            return;                             \
        else                                    \
            state.name = (newval);              \
    } while(0);

void gk_gl_reset_state(gk_context *gk) {
    auto &state = gk->impl_data->glstate;

    memset(&state, 0, sizeof(state));
}

void gk_glActiveTexture(gk_context *gk, GLenum texture) {
    auto &state = gk->impl_data->glstate;

    CHECK_SET_STATE(state, active_texture, texture);
    glActiveTexture(texture);
}

void gk_glUseProgram(gk_context *gk, GLuint program) {
    auto &state = gk->impl_data->glstate;

    CHECK_SET_STATE(state, active_program, program);
    glUseProgram(program);
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
