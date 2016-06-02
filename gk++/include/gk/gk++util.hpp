#pragma once

#include <string.h>

namespace gk {
    struct vec2 : public gk_vec2 {
        vec2() = default;
        vec2(float x_, float y_) { x = x_; y = y_; }
    };

    struct vec3 : public gk_vec3 {
        vec3() = default;
        vec3(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }
    };

    struct vec4 : public gk_vec4 {
        vec4() = default;
        vec4(float x_, float y_, float z_, float w_) {
            x = x_; y = y_; z = z_; w = w_;
        }
    };

    struct mat4 : public gk_mat4 {
        mat4() = default;
        mat4(int x) {
            memset(this, 0, sizeof(mat4));
            a00 = a11 = a22 = a33 = 1;
        }
    };
}
