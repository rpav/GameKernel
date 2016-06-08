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

    struct UniformValue : public gk_uniform_value {
        UniformValue() = default;

        UniformValue(gk_uniform location_,
                     gk_uniform_value_type type_,
                     size_t count_ = 1) {
            location = location_;
            type = type_;
            count = count_;
            value.data = nullptr;
        }

        UniformValue(gk_uniform location_, float f)
            : UniformValue(location_, GK_UNIFORM_VALUE_FLOAT) {
            value.f = f;
        }

        UniformValue(gk_uniform location_, int i)
            : UniformValue(location_, GK_UNIFORM_VALUE_INT) {
            value.i = i;
        }

        UniformValue(gk_uniform location_, unsigned int ui)
            : UniformValue(location_, GK_UNIFORM_VALUE_UINT) {
            value.ui = ui;
        }

        UniformValue(gk_uniform location_, gk_vec2 &v)
            : UniformValue(location_, GK_UNIFORM_VALUE_FLOAT) {
            value.data = &v;
        }

        UniformValue(gk_uniform location_, gk_vec3 &v)
            : UniformValue(location_, GK_UNIFORM_VALUE_FLOAT) {
            value.data = &v;
        }

        UniformValue(gk_uniform location_, gk_vec4 &v)
            : UniformValue(location_, GK_UNIFORM_VALUE_FLOAT) {
            value.data = &v;
        }

        UniformValue(gk_uniform location_, gk_mat4 &v)
            : UniformValue(location_, GK_UNIFORM_VALUE_MAT4) {
            value.data = &v;
        }
    };
}
