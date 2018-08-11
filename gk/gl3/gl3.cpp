#include <GL/glew.h>
#include <GL/gl.h>
#include <cstring>

#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/log.hpp"

#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

void gk_create_gl3(gk_context *gk) {
    gl3_impl *gl3 = new gl3_impl;

    gk->impl_data = gl3;
    gk->nvg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    gl3_quad_init(gk);
    gl3_rt_init(gk);
}

void gk_destroy_gl3(gk_context *gk) {
    auto gl3 = (gl3_impl*)gk->impl_data;

    gl3->quad_state.clean(gl3->glstate);

    delete[] gl3->quadbuf;
    delete gl3;

    nvgDeleteGL3(gk->nvg);

 gl_error:
    return;
}
