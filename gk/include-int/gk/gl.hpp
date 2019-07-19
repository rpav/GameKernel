#pragma once

#include <GL/glew.h>

#include <GL/gl.h>
#include <rpav/log.hpp>

#include "gk/gk++util.hpp"
#include "gk/gk.hpp"
#include "gk/glprogram.hpp"
#include "gk/glutil.hpp"

using namespace rpav;

/* Structs */
namespace gk {


struct QuadProgram : public GLProgram {
    GLuint uTEX;

    virtual void postLink() { uTEX = uniforms.add(findUniform("tex"), 0); }
};
} // namespace gk

struct gl3_impl : public gl_impl_data {
    // Quads
    gk::QuadProgram quad_program;

    int    quadcount;
    float* quadbuf;
};

/* Functions */
std::ostream& operator<<(std::ostream& os, const gk::mat4& m);

bool gk_init_gl(gk_context* gk);
void gk_fini_gl(gk_context* gk);

void gk_process_gl(gk_context* gk, gk_bundle* bundle, gk_list_gl* list);
void gk_process_cmd_tf_trs(gk_context* gk, gk_cmd_tf_trs* cmd);
void gk_process_cmd_tf_ortho(gk_context* gk, gk_cmd_tf_ortho* cmd);
void gk_process_cmd_tf_m(gk_context*, gk_cmd_tf_m* cmd);

// Any GL
void gl_cmd_clear(gk_context* gk, gk_cmd_clear* cmd);
void gl_cmd_program_create(gk_context* gk, gk_cmd_program_create* cmd);
void gl_cmd_program_destroy(gk_context* gk, gk_cmd_program_destroy* cmd);
void gl_cmd_uniform_query(gk_context* gk, gk_cmd_uniform_query* cmd);
void gl_cmd_default_pds(gk_context* gk, gk_cmd_default_pds* cmd);

/* GL2 */
void gk_create_gl2(gk_context* gk);
void gk_destroy_gl2(gk_context* gk);

/* GL3 */
void gk_create_gl3(gk_context* gk);
void gk_destroy_gl3(gk_context* gk);

void gl3_quad_init(gk_context*);
void gl3_begin_quad(gk_context*);
void gl3_cmd_quad(gk_context*, gk_bundle*, gk_cmd_quad*);
void gl3_cmd_quadsprite(gk_context*, gk_bundle*, gk_cmd_quadsprite*);
void gl3_end_quad(gk_context*);

void gl3_cmd_spritelayer(gk_context*, gk_bundle*, gk_cmd_spritelayer*);
void gl3_cmd_chunklayer(gk_context*, gk_bundle*, gk_cmd_chunklayer*);

void gl3_rt_init(gk_context*);
void gl3_cmd_rt_create(gk_context*, gk_cmd_rt_create*);
void gl3_cmd_rt_destroy(gk_context*, gk_cmd_rt_destroy*);
void gl3_cmd_rt_bind(gk_context*, gk_cmd_rt_bind*);
void gl3_cmd_rt_unbind(gk_context*, gk_cmd_rt_unbind*);

void gl3_cmd_default_pds(gk_context*, gk_cmd_default_pds*);

// tables
extern const GLenum gk_filter_to_gl[];
