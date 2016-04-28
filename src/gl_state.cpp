#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/log.hpp"

#define GLSTATE(gk) (*((gl_state*)((gk)->impl_data)))

#define CHECK_SET_STATE(name, newval)           \
    do {                                        \
        if((newval) == GLSTATE(gk).name)        \
            return;                             \
        else                                    \
            GLSTATE(gk).name = (newval);        \
    } while(0);

void gk_gl_reset_state(gk_context *gk) {
    GLSTATE(gk).active_program = 0;
    GLSTATE(gk).active_texture = 0;
    GLSTATE(gk).bound_array = 0;
    GLSTATE(gk).bound_array_buffer = 0;
}

void gk_glActiveTexture(gk_context *gk, GLenum texture) {
    CHECK_SET_STATE(active_texture, texture);
    glActiveTexture(texture);
}

void gk_glUseProgram(gk_context *gk, GLuint program) {
    CHECK_SET_STATE(active_program, program);
    glUseProgram(program);
}

void gk_glBindVertexArray(gk_context *gk, GLuint array) {
    CHECK_SET_STATE(bound_array, array);
    glBindVertexArray(array);
}

void gk_glBindBuffer(gk_context *gk, GLenum target, GLuint buffer) {
    switch(target) {
        case GL_ARRAY_BUFFER:
            CHECK_SET_STATE(bound_array_buffer, buffer);
            break;
    }
    glBindBuffer(target, buffer);
}
