#include <GL/glew.h>
#include <GL/gl.h>
#include <cstring>

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

static const char *shader_geom_quad =
 "#version 330 core\n"

 "layout(lines_adjacency) in;"
 "layout(triangle_strip, max_vertices = 4) out;"

 "in vec2 vert_uv[];"
 "out vec2 uv;"

 "void main() {"
 "    uv = vert_uv[0];"
 "    gl_Position = gl_in[0].gl_Position;"
 "  EmitVertex();"
 "    uv = vert_uv[1];"
 "    gl_Position = gl_in[1].gl_Position;"
 "  EmitVertex();"
 "    uv = vert_uv[2];"
 "    gl_Position = gl_in[2].gl_Position;"
 "  EmitVertex();"
 "    uv = vert_uv[3];"
 "    gl_Position = gl_in[3].gl_Position;"
 "  EmitVertex();"
 
 "  EndPrimitive();"
 "}"
 ;

static const char *shader_vert_quad =
 "#version 330 core\n"

 "layout (location = 0) in vec4 vertex;"
 "layout (location = 1) in vec2 in_uv;"

 "out vec2 vert_uv;"

 "void main() {"
 "  vert_uv = in_uv;"
 "  gl_Position = vertex;"
 "}"
 ;

static const char *shader_frag_quad =
 "#version 330 core\n"

 "uniform sampler2D tex;"

 "in vec2 uv;"
 "out vec4 frag;"

 "void main() {"
 "  frag = texture(tex, uv);"
//"  frag = vec4(1, 0, 0, 1);"
 "  if(frag.a == 0.0) discard;"
 "}"
 ;

const int QUADBUF_QUADS = 1024;
const int QUADBUF_VALS_PER_VERT = sizeof(gk_quadvert)/sizeof(float);

static void compile_shaders(gl3_impl *gl3) {
    auto vert = gk_gl_compile_shader(GL_VERTEX_SHADER, shader_vert_quad);
    auto geom = gk_gl_compile_shader(GL_GEOMETRY_SHADER, shader_geom_quad);
    auto frag = gk_gl_compile_shader(GL_FRAGMENT_SHADER, shader_frag_quad);

    GLuint shaders[] = { vert, geom, frag };
    gl3->prog_quad = gk_gl_link_program(3, shaders);
    gl3->quad_uTEX = glGetUniformLocation(gl3->prog_quad, "tex"); GL_CHECKERR(glGetUniformLocation);

    return;

 gl_error:
    return;
}

void gl3_quad_init(gk_context *gk) {
    const size_t szf = sizeof(float);

    auto gl3 = (gl3_impl*)gk->impl_data;
    gl3->quadbuf = new float[QUADBUF_QUADS * QUADBUF_VALS_PER_VERT * 4];

    gk->gl.gl_begin_quad = gl3_begin_quad;
    gk->gl.gl_cmd_quad = gl3_cmd_quad;
    gk->gl.gl_cmd_quadsprite = gl3_cmd_quadsprite;
    gk->gl.gl_end_quad = gl3_end_quad;

    compile_shaders(gl3);

    GL_CHECK(glGenVertexArrays(1, &gl3->quadvao));
    GL_CHECK(glGenBuffers(1, &gl3->quadvbo));
    GL_CHECK(glBindVertexArray(gl3->quadvao));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, gl3->quadvbo));
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

void gl3_begin_quad(gk_context *gk, gk_bundle *b, gk_cmd_quad *q) {
    auto gl3 = (gl3_impl*)gk->impl_data;
    gl3->quadcount = 0;
    gl3->tex = 0;

    GL_CHECK(gk_glUseProgram(gk, gl3->prog_quad));
    GL_CHECK(gk_glBindVertexArray(gk, gl3->quadvao));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, gl3->quadvbo));
    GL_CHECK(glUniform1i(gl3->quad_uTEX, 0));

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

static inline bool gl3_quad_ensuretex(gk_context *gk, int tex) {
   auto gl3 = (gl3_impl*)gk->impl_data;

    if(tex != gl3->tex) {
        gl3_render_quads(gk);
        gl3->tex = tex;

        GL_CHECK(gk_glActiveTexture(gk, GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, gl3->tex));
    }
    return true;

 gl_error:
    return false;
}

void gl3_cmd_quad(gk_context *gk, gk_bundle *b, gk_cmd_quad *q) {
    if(!gl3_quad_ensuretex(gk, q->tex)) return;
    
    gl3_append_quad(gk, (mat4*)&q->tfm, q->attr);
}

void gl3_cmd_quadsprite(gk_context *gk, gk_bundle *b, gk_cmd_quadsprite *cmd) {
    auto sheet = cmd->sheet;
    auto &sprite = cmd->sheet->sprites[cmd->index];

    if(!gl3_quad_ensuretex(gk, sheet->tex)) return;

    gl3_append_quad(gk, (mat4*)&cmd->tfm, sprite.attr);
}

void gl3_end_quad(gk_context *gk) {
    gl3_render_quads(gk);
}
