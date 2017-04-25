#if defined(WIN32) || defined(_WIN32)
#  include <windows.h>
#  undef near
#  undef far
#endif
 
#include <GL/glew.h>
#include "nanovg.h"

#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/nvg.hpp"
#include "gk/log.hpp"

extern "C" GK_API gk_context* gk_create(gk_impl impl) {
    auto *gk = new gk_context;

    gk->impl = impl;
    gk->last_sub = GK_SUB_NULL;
    gk->current_bundle = nullptr;

    if(!gk_init_gl(gk)) goto error;

    return gk;

 error:
    delete gk;
    return nullptr;
}

extern "C" GK_API void gk_destroy(gk_context *gk) {
    gk_fini_gl(gk);
}

extern "C" GK_API void gk_process(gk_context *gk, gk_bundle *b) {
    gk->current_bundle = b;
    gk_seterror(gk, GK_ERROR_NONE);

    gk_process_pass(gk, b, &b->start);
    gk->current_bundle = nullptr;
}
