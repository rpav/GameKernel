#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using mat4 = glm::mat4;
using vec4 = glm::vec4;
using vec3 = glm::vec3;
using vec2 = glm::vec2;

#include <cstring>

#define GK_GLM_CONVERSIONS

#include "gk/gk.h"
#include "gk/gl.hpp"
#include "gk/log.hpp"

const mat4 I4(1.0);

void gk_vec2::set(float x_, float y_) { x = x_; y = y_; }
void gk_vec3::set(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }
void gk_vec4::set(float x_, float y_, float z_, float w_) { x = x_; y = y_; z = z_; w = w_; }

void gk_vec2::set(float *ptr) { memcpy(this, ptr, sizeof(gk_vec2)); }
void gk_vec3::set(float *ptr) { memcpy(this, ptr, sizeof(gk_vec3)); }
void gk_vec4::set(float *ptr) { memcpy(this, ptr, sizeof(gk_vec4)); }
void gk_mat4::set(float *ptr) { memcpy(this, ptr, sizeof(gk_mat4)); }

void gk_process_cmd_tf_trs(gk_context *, gk_cmd_tf_trs *cmd) {
    const mat4 *prior = (mat4*)cmd->parent.prior;
    mat4 *tf = (mat4*)cmd->parent.out;

    if(!prior) prior = &I4;

    *tf = glm::translate<float>(*prior, glm::make_vec3((float*)&cmd->translate));

    if(cmd->flags & GK_TRS_ROTATE)
        *tf = glm::rotate<float>(*tf, cmd->angle, glm::make_vec3((float*)&cmd->axis));

    if(cmd->flags & GK_TRS_SCALE)
        *tf = glm::scale<float>(*tf, glm::make_vec3((float*)&cmd->scale));
}

void gk_process_cmd_tf_ortho(gk_context *, gk_cmd_tf_ortho *cmd) {
    mat4 *tf = (mat4*)cmd->parent.out;

    *tf = glm::ortho<float>(cmd->left, cmd->right, cmd->bottom, cmd->top,
                            cmd->near, cmd->far);
}

void gk_process_cmd_tf_m(gk_context *, gk_cmd_tf_m *cmd) {
    glm::mat4 &prior = *cmd->parent.prior;
    glm::mat4 &m     = *cmd->m;
    glm::mat4 &out   = *cmd->parent.out;

    out = prior * m;
}