#include <GL/glew.h>
#include <GL/gl.h>
#include "gk/gk.hpp"

#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"

void gk_create_gl2(gk_context *gk) {
    gk->nvg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
}

void gk_destroy_gl2(gk_context *gk) {
    nvgDeleteGL2(gk->nvg);
}
