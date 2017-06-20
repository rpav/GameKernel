#include <GL/glew.h>
#include <GL/gl.h>
#include <cstring>

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/log.hpp"

using mat4 = glm::mat4;
using vec4 = glm::vec4;
using vec3 = glm::vec3;
using vec2 = glm::vec2;

static const char *shader_geom_quad = R"(
#version 330 core

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 vert_uv[];
out vec2 uv;

void main() {
    uv = vert_uv[0];
    gl_Position = gl_in[0].gl_Position;
	//gl_Position = vec4(-1,-1,0,0);
  EmitVertex();
    uv = vert_uv[1];
    gl_Position = gl_in[1].gl_Position;
	//gl_Position = vec4(-1,1,0,0);
  EmitVertex();
    uv = vert_uv[2];
    gl_Position = gl_in[2].gl_Position;
	//gl_Position = vec4(1,-1,0,0);
  EmitVertex();
    uv = vert_uv[3];
    gl_Position = gl_in[3].gl_Position;
	//gl_Position = vec4(1,1,0,0);
  EmitVertex();

  EndPrimitive();
}
)";

static const char *shader_vert_quad = R"(
#version 330 core

layout (location = 0) in vec4 vertex;
layout (location = 1) in vec2 in_uv;

out vec2 vert_uv;

void main() {
  vert_uv = in_uv;
  gl_Position = vertex;
}
)";

static const char *shader_frag_quad = R"(
#version 330 core

uniform sampler2D tex;

in vec2 uv;
out vec4 frag;

void main() {
  frag = texture(tex, uv);
  //frag = vec4(1, 0, 0, 1);
  if(frag.a == 0.0) discard;
}
)";

const int QUADBUF_QUADS = 1024;
const int QUADBUF_VALS_PER_VERT = sizeof(gk_quadvert)/sizeof(float);

static void compile_shaders(gl3_impl *gl3) {
    gk::GLProgramBuilder build;

    build.add(GL_VERTEX_SHADER, shader_vert_quad,
              GL_GEOMETRY_SHADER, shader_geom_quad,
              GL_FRAGMENT_SHADER, shader_frag_quad);
    build.link(gl3->quad_program);

    // Default program data set
    gl3->quad_state.default_pds.set(gl3->quad_program);
    gl3->quad_state.default_pds.set(gl3->quad_program.uniforms);

    // Start with the default
    gl3->quad_state.pds.set(gl3->quad_state.default_pds, gl3->quad_state);
}

void gl3_quad_init(gk_context *gk) {
    const size_t szf = sizeof(float);

    auto gl3 = (gl3_impl*)gk->impl_data;
    auto &qs = gl3->quad_state;

    gl3->quadbuf = new float[QUADBUF_QUADS * QUADBUF_VALS_PER_VERT * 4];

    gk->gl.gl_begin_quad = gl3_begin_quad;
    gk->gl.gl_cmd_quad = gl3_cmd_quad;
    gk->gl.gl_cmd_quadsprite = gl3_cmd_quadsprite;
    gk->gl.gl_end_quad = gl3_end_quad;
    gk->gl.gl_cmd_spritelayer = gl3_cmd_spritelayer;

    compile_shaders(gl3);

    GLuint vao, vbo;

    GL_CHECK(glGenVertexArrays(1, &vao));
    GL_CHECK(glGenBuffers(1, &vbo));

    qs.vao.set(vao, qs);
    qs.vbo.set(vbo, qs);

    qs.apply(gk->impl_data->glstate);

    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, QUADBUF_VALS_PER_VERT*szf,
                                   (void*)0));
    GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, QUADBUF_VALS_PER_VERT*szf,
                                   (void*)(szf*4)));

    return;

 gl_error:
    return;
}

void gl3_render_quads(gk_context *gk) {
    auto gl3 = (gl3_impl*)gk->impl_data;

    if(gl3->quadcount > 0) {
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER,
                              gl3->quadcount * (sizeof(float) * QUADBUF_VALS_PER_VERT * 4),
                              gl3->quadbuf, GL_STREAM_DRAW));
        GL_CHECK(glDrawArrays(GL_LINES_ADJACENCY, 0, gl3->quadcount * 4));
        gl3->quadcount = 0;
    }
    return;

 gl_error:
    return;
}

void gl3_begin_quad(gk_context *gk) {
    auto gl3 = (gl3_impl*)gk->impl_data;
    auto &config = gl3->quad_state;

    config.dirty = true;
    config.apply(gl3->glstate);

    gl3->quadcount = 0;

    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

 gl_error:
    return;
}

static void gl3_append_quad(gk_context *gk, mat4 *tfm, gk_quadvert *attr) {
    auto gl3 = (gl3_impl*)gk->impl_data;
    gk_quadvert *out = (gk_quadvert*)(gl3->quadbuf + (gl3->quadcount * QUADBUF_VALS_PER_VERT * 4));

    *(vec4*)(&out[0].vertex) = (*tfm) * *(vec4*)&attr[0].vertex;
    *(vec4*)(&out[1].vertex) = (*tfm) * *(vec4*)&attr[1].vertex;
    *(vec4*)(&out[2].vertex) = (*tfm) * *(vec4*)&attr[2].vertex;
    *(vec4*)(&out[3].vertex) = (*tfm) * *(vec4*)&attr[3].vertex;
    out[0].uv = attr[0].uv;
    out[1].uv = attr[1].uv;
    out[2].uv = attr[2].uv;
    out[3].uv = attr[3].uv;

    gl3->quadcount++;
    if(gl3->quadcount >= QUADBUF_QUADS)
        gl3_render_quads(gk);
}

static inline void gl3_quad_ensure_state(gk_context *gk, GLuint tex,
                                         gk_program_data_set *pds) {
    auto gl3 = (gl3_impl*)gk->impl_data;
    auto &config = gl3->quad_state;

    config.tex.set(tex, config);

    if(pds) {
        config.pds.set(pds, config);
    } else {
        config.pds.set(gl3->quad_state.default_pds, config);
    }

    if(config.dirty) {
        gl3_render_quads(gk);
        config.apply(gl3->glstate);
    }
}

void gl3_cmd_quad(gk_context *gk, gk_bundle *, gk_cmd_quad *q) {
    gl3_quad_ensure_state(gk, q->tex, q->pds);
    gl3_append_quad(gk, (mat4*)&q->tfm, q->attr);
}

void gl3_cmd_quadsprite(gk_context *gk, gk_bundle *, gk_cmd_quadsprite *cmd) {
    auto sheet = cmd->sheet;
    auto &sprite = cmd->sheet->sprites[cmd->index];

    gl3_quad_ensure_state(gk, sheet->tex, cmd->pds);
    gl3_append_quad(gk, (mat4*)&cmd->tfm, sprite.attr);
}

void gl3_end_quad(gk_context *gk) {
    auto gl3 = (gl3_impl*)gk->impl_data;
    gl3_render_quads(gk);
}

void gl3_cmd_spritelayer(gk_context *gk, gk_bundle *b, gk_cmd_spritelayer *cmd) {
    auto *sheet = cmd->sheet;
    mat4 m;
    vec3 tr;

    gl3_quad_ensure_state(gk, sheet->tex, cmd->pds);
    auto layer_x = (size_t)cmd->layer_size.x;
    auto sx = cmd->sprite_size.x;
    auto sy = cmd->sprite_size.y;
    auto *layer_m = (mat4*)cmd->tfm;

    int si=0,bx=0;
    int sj=0,by=0;

    if(cmd->flags & GK_SPRITELAYER_FLIPX) {
        si = glm::clamp<int>(cmd->layer_size.x - cmd->bounds.z, 0, cmd->layer_size.x);
        bx = glm::clamp<int>(si + cmd->bounds.z, 0, cmd->layer_size.x);
    } else {
        si = glm::clamp<int>(cmd->bounds.x, 0, cmd->layer_size.x);
        bx = glm::clamp<int>(si + cmd->bounds.z, 0, cmd->layer_size.x);
    }

    if(cmd->flags & GK_SPRITELAYER_FLIPY) {
        sj = glm::clamp<int>(cmd->layer_size.y - cmd->bounds.w, 0, cmd->layer_size.y);
        by = glm::clamp<int>(sj + cmd->bounds.w, 0, cmd->layer_size.y);
    } else {
        sj = glm::clamp<int>(cmd->bounds.y, 0, cmd->layer_size.y);
        by = glm::clamp<int>(sj + cmd->bounds.w, 0, cmd->layer_size.y);
    }

    for(int j = sj; j < by; ++j) {
        for(int i = si; i < bx; ++i) {
            tr.x = i * sx;
            tr.y = j * sy;

            size_t index = 0;

            if(cmd->flags & GK_SPRITELAYER_FLIPY)
                index = (by-j-1)*layer_x;
            else
                index = j*layer_x;

            index += i;

            size_t spriteno = cmd->sprites[index];

            if(!spriteno) continue;
            
            m = glm::translate(*layer_m, tr);
            auto &sprite = sheet->sprites[spriteno-1];

            gl3_append_quad(gk, &m, sprite.attr);
        }
    }
}