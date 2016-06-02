#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

/* For state-aware GL wrappers.  This MUST be the FIRST member for all
   impl_data for GL. */
struct gl_state {
    GLuint active_program;
    GLenum active_texture;

    GLuint bound_array;
    GLuint bound_array_buffer;
};

/* Structs */
struct gl3_impl {
    gl_state glstate;

    // Quads
    GLuint tex;
    int prog_quad;
    GLuint quad_uMVP;
    GLuint quad_uTEX;

    int quadcount;
    float *quadbuf;
    GLuint quadvao;
    GLuint quadvbo;
};

/* Functions */

#ifndef NDEBUG
#define GL_CHECK(x) do { x; if(gk_gl_checkerror(#x,__FILE__,__LINE__)) goto gl_error; } while(0)
#define GL_CHECKERR(x) do { if(gk_gl_checkerror(#x,__FILE__,__LINE__)) goto gl_error; } while(0)
#else
#define GL_CHECK(x) (x)
#define GL_CHECKERR(x)
#endif

bool gk_gl_checkerror(const char *expr, const char *file, int line);

GLuint gk_gl_compile_shader(GLenum type, const char *text);
GLuint gk_gl_link_program(int numshaders, GLuint *shaders);

bool gk_init_gl(gk_context *gk);
void gk_fini_gl(gk_context *gk);

void gk_process_gl(gk_context *gk, gk_bundle *bundle, gk_list_gl *list);
void gk_process_cmd_tf_trs(gk_context *gk, gk_cmd_tf_trs *cmd);
void gk_process_cmd_tf_ortho(gk_context *gk, gk_cmd_tf_ortho *cmd);

// Any GL
void gl_cmd_clear(gk_context *gk, gk_cmd_clear *cmd);

/* GL2 */
void gk_create_gl2(gk_context *gk);
void gk_destroy_gl2(gk_context *gk);

/* GL3 */
void gk_create_gl3(gk_context *gk);
void gk_destroy_gl3(gk_context *gk);

void gl3_quad_init(gk_context*);
void gl3_begin_quad(gk_context*, gk_bundle*, gk_cmd_quad*);
void gl3_cmd_quad(gk_context*, gk_bundle*, gk_cmd_quad*);
void gl3_cmd_quadsprite(gk_context*, gk_bundle*, gk_cmd_quadsprite*);
void gl3_end_quad(gk_context*);

void gl3_rt_init(gk_context*);
void gl3_cmd_rt_create(gk_context*, gk_cmd_rt_create*);
void gl3_cmd_rt_destroy(gk_context*, gk_cmd_rt_destroy*);
void gl3_cmd_rt_bind(gk_context*, gk_cmd_rt_bind*);
void gl3_cmd_rt_unbind(gk_context*, gk_cmd_rt_unbind*);

void gk_gl_reset_state(gk_context *gk);

void gk_glActiveTexture(gk_context *gk, GLenum texture);
void gk_glUseProgram(gk_context *gk, GLuint program);

void gk_glBindVertexArray(gk_context *gk, GLuint array);
void gk_glBindBuffer(gk_context *gk, GLenum target, GLuint buffer);

// tables

extern const GLenum gk_filter_to_gl[];
