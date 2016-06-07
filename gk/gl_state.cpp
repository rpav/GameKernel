#include <string.h>

#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/log.hpp"

// This has to correspond exactly to `enum gk::BufferTarget`
const GLenum gk::target_to_gl[] = {
    GL_ARRAY_BUFFER,
    GL_ATOMIC_COUNTER_BUFFER,
    GL_COPY_READ_BUFFER,
    GL_COPY_WRITE_BUFFER,
    GL_DISPATCH_INDIRECT_BUFFER,
    GL_DRAW_INDIRECT_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER,
    GL_PIXEL_PACK_BUFFER,
    GL_PIXEL_UNPACK_BUFFER,
    GL_QUERY_BUFFER,
    GL_SHADER_STORAGE_BUFFER,
    GL_TEXTURE_BUFFER,
    GL_TRANSFORM_FEEDBACK_BUFFER,
    GL_UNIFORM_BUFFER,

    0
};;

void gk_gl_reset_state(gk_context *gk) {
    auto &state = gk->impl_data->glstate;
    state.reset();
}

void gk::GLGlobalState::reset() {
    active_program = 0;
    active_texture = 0;
    bound_vertex_array = 0;

    textures.clear();

    for(int i = 0; i < gk::BUFFER_MAX; ++i)
        buffers[i] = 0;
}
