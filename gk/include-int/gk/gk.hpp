#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

#include "gk/gk.h"
#include "gk/glstate.hpp"

typedef struct NVGcontext NVGcontext;

// Base for implementations
struct gl_impl_data {
    gk::GLGlobalState glstate;
};

struct gk_context {
    gk_impl impl;
    gl_impl_data *impl_data;

    NVGcontext *nvg;
    bool nvg_inframe;

    gk_subsystem last_sub;

    struct {
        gk_cmd_type last_cmd;

        void (*gl_begin_quad)(gk_context*);
        void (*gl_cmd_quad)(gk_context*, gk_bundle*, gk_cmd_quad*);
        void (*gl_cmd_quadsprite)(gk_context*, gk_bundle*, gk_cmd_quadsprite*);
        void (*gl_cmd_spritelayer)(gk_context*, gk_bundle*, gk_cmd_spritelayer*);
        void (*gl_end_quad)(gk_context*);

        void (*gl_cmd_rt_create)(gk_context*, gk_cmd_rt_create*);
        void (*gl_cmd_rt_destroy)(gk_context*, gk_cmd_rt_destroy*);
        void (*gl_cmd_rt_bind)(gk_context*, gk_cmd_rt_bind*);
        void (*gl_cmd_rt_unbind)(gk_context*, gk_cmd_rt_unbind*);
    } gl;

    gk_bundle *current_bundle;
};

bool gk_mark(gk_context *gk, gk_list *list);
bool gk_unmark(gk_context *gk, gk_list *list);

extern const char *gk_error_strings[];

void gk_seterror(gk_context *gk, gk_error_code code);
bool gk_haserror(gk_context *gk);

bool gk_process_pass(gk_context *gk, gk_bundle *b, gk_pass *pass);
bool gk_process_list(gk_context *gk, gk_bundle *b, gk_list *list);
void gk_process_config(gk_context *gk, gk_bundle *b, gk_list *pass);

bool gk_process_cmd_general(const char *listname, gk_context *gk, gk_bundle *b, gk_cmd *cmd);
