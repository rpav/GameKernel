#include <cstring>

#include <rpav/log.hpp>

#include "gk/gk.h"
#include "gk/gl.hpp"

using namespace rpav;

void gk_process_cmd_tf_trs(gk_context*, gk_cmd_tf_trs* cmd)
{
    const auto* prior = cmd->parent.prior;
    auto*       tf    = cmd->parent.out;

    if(!prior) prior = &gk::IDENTITY;

    *tf = *prior * gk::mat4::translate(cmd->translate);

    if(cmd->flags & GK_TRS_ROTATE) *tf = *tf * gk::mat4::rotate(cmd->angle, cmd->axis);

    if(cmd->flags & GK_TRS_SCALE) *tf = *tf * gk::mat4::scale(cmd->scale);
}

void gk_process_cmd_tf_ortho(gk_context*, gk_cmd_tf_ortho* cmd)
{
    auto* tf = cmd->parent.out;

    *tf = gk::mat4::ortho(cmd->left, cmd->right, cmd->bottom, cmd->top, cmd->near,
                          cmd->far);
}

void gk_process_cmd_tf_m(gk_context*, gk_cmd_tf_m* cmd)
{
    *cmd->parent.out = *cmd->parent.prior * *cmd->m;
}