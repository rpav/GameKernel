/******************************************************************
 * Basic math types.  These are generally compatible with other
 * similar implementations and often treated that way (i.e. zero
 * copy).
 ******************************************************************/

#ifndef __GAMEKERNEL_MATH_H__
#define __GAMEKERNEL_MATH_H__

#ifdef __cplusplus
#define GK_CXX_API GK_API
#include <string.h>
#include <math.h>
#else
#define CXX_SET(...)
#define GK_CXX_API
#endif

typedef struct GK_CXX_API gk_vec2 {
    float x, y;

#ifdef __cplusplus
    gk_vec2() = default;
    gk_vec2(int) { x = 0; y = 0; }
    gk_vec2(float x_, float y_) : x(x_), y(y_) { }
    gk_vec2(const gk_vec2 &v) { x = v.x; y = v.y; }

    gk_vec2 operator+(const gk_vec2 &v) const { return gk_vec2(x + v.x, y + v.y); }
    gk_vec2& operator+=(const gk_vec2 &v) { x += v.x; y += v.y; return *this; }

    gk_vec2 operator-(const gk_vec2 &v) const { return gk_vec2(x - v.x, y - v.y); }
    gk_vec2& operator-=(const gk_vec2 &v) { x -= v.x; y -= v.y; return *this; }

    gk_vec2 operator*(float f) const { return gk_vec2(x * f, y * f); }
    gk_vec2& operator*=(float f) { x *= f; y *= f; return *this; }

    gk_vec2 operator/(float f) const { return gk_vec2(x / f, y / f); }
    gk_vec2& operator/=(float f) { x /= f; y /= f; return *this; }

    gk_vec2& operator=(const gk_vec2 &v) { x = v.x; y = v.y; return *this; }

    void set(float x, float y);
    void set(float*);

    // Note: These are simple but not terribly efficient

    // Requires a normalized vector
    float angle(const gk_vec2 &v) const { return atan2(v.y, v.x) - atan2(y, x); }

    float length() const {
        return sqrt((x*x) + (y*y));
    }

    gk_vec2& normalize() {
        float scale = 1.0f / length();

        x *= scale;
        y *= scale;

        return *this;
    }

    gk_vec2& rotate(float radians) {
        float c = cos(radians);
        float s = sin(radians);

        float _x = x*c - y*s;
        float _y = x*s + y*c;

        x = _x;
        y = _y;

        return *this;
    }
#endif
} gk_vec2;

typedef struct GK_CXX_API gk_vec3 {
    float x, y, z;

#ifdef __cplusplus
    gk_vec3() = default;
    gk_vec3(int) { x = 0; y = 0; z = 0; }
    gk_vec3(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }

    gk_vec3 operator+(const gk_vec3 &v) { return gk_vec3(x + v.x, y + v.y, z + v.z); }
    gk_vec3& operator+=(const gk_vec3 &v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }

    gk_vec3 operator-(const gk_vec3 &v) { return gk_vec3(x - v.x, y - v.y, z - v.z); }
    gk_vec3& operator-=(const gk_vec3 &v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }

    void set(float x, float y, float z);
    void set(float*);
#endif
} gk_vec3;

typedef struct GK_CXX_API gk_vec4 {
    float x, y, z, w;

#ifdef __cplusplus
    gk_vec4() = default;
    gk_vec4(int) { x = 0; y = 0; z = 0; w = 0; }
    gk_vec4(float x_, float y_, float z_, float w_) {
        x = x_; y = y_; z = z_; w = w_;
    }

    gk_vec4 operator+(const gk_vec4 &v) { return gk_vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
    gk_vec4& operator+=(const gk_vec4 &v) {
        x += v.x; y += v.y; z += v.z; w += v.w;
        return *this;
    }

    gk_vec4 operator-(const gk_vec4 &v) { return gk_vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
    gk_vec4& operator-=(const gk_vec4 &v) {
        x -= v.x; y -= v.y; z -= v.z; w -= v.w;
        return *this;
    }

    void set(float x, float y, float z, float w);
    void set(float*);
#endif
} gk_vec4;

typedef struct GK_CXX_API gk_mat4 {
    float a00, a01, a02, a03;
    float a10, a11, a12, a13;
    float a20, a21, a22, a23;
    float a30, a31, a32, a33;

#ifdef __cplusplus
    gk_mat4() = default;
    gk_mat4(int) {
        memset(this, 0, sizeof(gk_mat4));
        a00 = a11 = a22 = a33 = 1;
    }

    void set(float*);
#endif

#if defined(__cplusplus) && defined(GK_GLM_CONVERSIONS)
    gk_mat4(const glm::mat4 &m) { *this = *(gk_mat4*)&m; }
    inline operator glm::mat4&() { return (glm::mat4&)*this; }
#endif
} gk_mat4;

#endif  /* __GAMEKERNEL_MATH_H__ */
