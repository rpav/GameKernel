#include <cstring>

#include <rpav/log.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using mat4 = glm::mat4;
using vec4 = glm::vec4;
using vec3 = glm::vec3;
using vec2 = glm::vec2;

#define GK_GLM_CONVERSIONS

#include "gk/gk.h"
#include "gk/gl.hpp"

using namespace rpav;

const mat4 I4(1.0);

void gk_process_cmd_tf_trs(gk_context*, gk_cmd_tf_trs* cmd)
{
    const mat4* prior = (mat4*)cmd->parent.prior;
    mat4*       tf    = (mat4*)cmd->parent.out;

    if(!prior) prior = &I4;

    *tf = glm::translate<float>(*prior, glm::make_vec3((float*)&cmd->translate));

    if(cmd->flags & GK_TRS_ROTATE)
        *tf = glm::rotate<float>(*tf, cmd->angle, glm::make_vec3((float*)&cmd->axis));

    if(cmd->flags & GK_TRS_SCALE)
        *tf = glm::scale<float>(*tf, glm::make_vec3((float*)&cmd->scale));
}

void gk_process_cmd_tf_ortho(gk_context*, gk_cmd_tf_ortho* cmd)
{
    mat4* tf = (mat4*)cmd->parent.out;

    *tf = glm::ortho<float>(cmd->left, cmd->right, cmd->bottom, cmd->top, cmd->near,
                            cmd->far);
}

void gk_process_cmd_tf_m(gk_context*, gk_cmd_tf_m* cmd)
{
    glm::mat4 prior = *cmd->parent.prior;
    glm::mat4 m     = *cmd->m;

    *cmd->parent.out = prior * m;
}