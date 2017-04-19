#include <algorithm>

#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/nvg.hpp"
#include "gk/spritesheet.hpp"
#include "gk/b2.hpp"
#include "gk/log.hpp"

bool gk_mark(gk_context *gk, gk_list *list) {
    if(list->in_use) {
        LOG("Error: recursively trying to process list ", list);
        gk_seterror(gk, GK_ERROR_LIST_RECURSION);
        return false;
    }
    list->in_use = true;
    return true;
}

bool gk_unmark(gk_context *gk, gk_list *list) {
    if(!list->in_use) {
        LOG("Error: trying to unmark already-unmarked list ", list);
        gk_seterror(gk, GK_ERROR_LIST_CORRUPT);
        return false;
    }
    list->in_use = false;
    return true;

}

void gk_seterror(gk_context *gk, gk_error_code c) {
    gk_bundle *b = gk->current_bundle;

    if(b) {
        b->error.code = c;
        b->error.message = gk_error_strings[c];
    }
}

bool gk_haserror(gk_context *gk) {
    return gk->current_bundle && (gk->current_bundle->error.code != GK_ERROR_NONE);
}

static bool pass_asc_f (gk_cmd *a, gk_cmd *b) { return a->key < b->key; }
static bool pass_desc_f(gk_cmd *a, gk_cmd *b) { return b->key < a->key; }

bool gk_process_pass(gk_context *gk, gk_bundle *b, gk_pass *pass) {
    if(pass->list_index >= b->nlists) {
        gk_seterror(gk, GK_ERROR_PASS_BADLIST);
        return false;
    }

    auto list = b->lists[pass->list_index];
    if(!gk_mark(gk, list)) return false;

    switch(pass->sort) {
        case GK_PASS_SORT_ASC:
            std::sort(list->cmds, list->cmds + list->ncmds, pass_asc_f);
            break;

        case GK_PASS_SORT_DESC:
            std::sort(list->cmds, list->cmds + list->ncmds, pass_desc_f);
            break;

        case GK_PASS_SORT_STABLE_ASC:
            std::stable_sort(list->cmds, list->cmds + list->ncmds, pass_asc_f);
            break;

        case GK_PASS_SORT_STABLE_DESC:
            std::stable_sort(list->cmds, list->cmds + list->ncmds, pass_desc_f);
            break;

        case GK_PASS_SORT_NONE:
        default:
            break;
    }

    gk_unmark(gk, list);
    return gk_process_list(gk, b, list);
}

bool gk_process_list(gk_context *gk, gk_bundle *b, gk_list *list) {
    if(!gk_mark(gk, list)) return false;

    if(list->sub != gk->last_sub) {
        gk->last_sub = list->sub;
        gk_gl_reset_state(gk);
    }

    switch(list->sub) {
        case GK_SUB_GL:
            gk_process_gl(gk, b, (gk_list_gl*)list);
            break;
        case GK_SUB_NVG:
            gk_process_nvg(gk, b, (gk_list_nvg*)list);
            break;
        case GK_SUB_BOX2D:
            gk_process_box2d(gk, b, list);
            break;
        case GK_SUB_CONFIG:
            gk_process_config(gk, b, list);
            break;
        default:
            LOG("Error: unknown list type: ", list->sub);
            gk_seterror(gk, GK_ERROR_LIST_UNKNOWN);
            gk_unmark(gk, list);
            return false;
    }

    gk_unmark(gk, list);
    return true;
}

void gk_process_config(gk_context *gk, gk_bundle *b, gk_list *list) {
    for(int i = 0; i < list->ncmds; ++i) {
        auto cmd = list->cmds[i];
        gk_process_cmd_general("GK_LIST_CONFIG", gk, b, cmd);
    }
}

bool gk_process_cmd_general(const char *listname, gk_context *gk, gk_bundle *b, gk_cmd *cmd) {

    switch(GK_CMD_TYPE(cmd)) {
        case GK_CMD_PASS:
            gk_process_pass(gk, b, (gk_pass*)cmd);
            break;

        case GK_CMD_TF_TRS:
            gk_process_cmd_tf_trs(gk, (gk_cmd_tf_trs*)cmd);
            break;
        case GK_CMD_TF_ORTHO:
            gk_process_cmd_tf_ortho(gk, (gk_cmd_tf_ortho*)cmd);
            break;

        case GK_CMD_CLEAR:
            gl_cmd_clear(gk, (gk_cmd_clear*)cmd);
            break;

        case GK_CMD_PROGRAM_CREATE:
            gl_cmd_program_create(gk, (gk_cmd_program_create*)cmd);
            break;
        case GK_CMD_PROGRAM_DESTROY:
            gl_cmd_program_destroy(gk, (gk_cmd_program_destroy*)cmd);
            break;
        case GK_CMD_UNIFORM_QUERY:
            gl_cmd_uniform_query(gk, (gk_cmd_uniform_query*)cmd);
            break;

        case GK_CMD_IMAGE_CREATE:
            gk_process_nvg_image_create(gk, (gk_cmd_image_create*)cmd);
            break;
        case GK_CMD_IMAGE_DESTROY:
            gk_process_nvg_image_destroy(gk, (gk_cmd_image_destroy*)cmd);
            break;
        case GK_CMD_FONT_CREATE:
            gk_process_nvg_font_create(gk, (gk_cmd_font_create*)cmd);
            break;
        case GK_CMD_FONT_FACE:
            gk_process_nvg_font_face(gk, (gk_cmd_font_face*)cmd);
            break;
        case GK_CMD_FONT_STYLE:
            gk_process_nvg_font_style(gk, (gk_cmd_font_style*)cmd);
            break;

        case GK_CMD_SPRITESHEET_CREATE:
            gk_process_spritesheet_create(gk, (gk_cmd_spritesheet_create*)cmd);
            break;
        case GK_CMD_SPRITESHEET_DESTROY:
            gk_process_spritesheet_destroy(gk, (gk_cmd_spritesheet_destroy*)cmd);
            break;

        case GK_CMD_RT_CREATE:
            gk->gl.gl_cmd_rt_create(gk, (gk_cmd_rt_create*)cmd);
            break;

        case GK_CMD_RT_DESTROY:
            gk->gl.gl_cmd_rt_destroy(gk, (gk_cmd_rt_destroy*)cmd);
            break;

        case GK_CMD_RT_BIND:
            gk->gl.gl_cmd_rt_bind(gk, (gk_cmd_rt_bind*)cmd);
            break;

        case GK_CMD_RT_UNBIND:
            gk->gl.gl_cmd_rt_unbind(gk, (gk_cmd_rt_unbind*)cmd);
            break;

#ifdef GK_USE_BOX2D
        case GK_CMD_B2_WORLD_CREATE:
            gk_process_b2_world_create(gk, (gk_cmd_b2_world_create*)cmd);
            break;

        case GK_CMD_B2_WORLD_DESTROY:
            gk_process_b2_world_destroy(gk, (gk_cmd_b2_world_destroy*)cmd);
            break;
#endif // GK_USE_BOX2D

        default:
            LOG("Unknown or invalid command type for ", listname, ": ",
                GK_CMD_TYPE(cmd));
            gk_seterror(gk, GK_ERROR_CMD_UNKNOWN);
            return false;
    }
    return true;
}
