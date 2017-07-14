/******************************************************************
 * Basic math types.  These are generally compatible with other
 * similar implementations and often treated that way (i.e. zero
 * copy).
 ******************************************************************/

#ifndef __GAMEKERNEL_MATH_H__
#define __GAMEKERNEL_MATH_H__

#ifdef __cplusplus
#define CXX_SET(...) void set(__VA_ARGS__)
#define GK_CXX_API GK_API
#else
#define CXX_SET(...)
#define GK_CXX_API
#endif

typedef struct GK_CXX_API gk_vec2 {
    float x, y;

    CXX_SET(float x, float y);
    CXX_SET(float*);

#ifdef __cplusplus
    gk_vec2() = default;

    gk_vec2(float x_, float y_) : x(x_), y(y_) { }

    gk_vec2 operator+(const gk_vec2 &v) const { return gk_vec2(x + v.x, y + v.y); }
    gk_vec2& operator+=(const gk_vec2 &v) { x += v.x; y += v.y; return *this; }

    gk_vec2 operator-(const gk_vec2 &v) const { return gk_vec2(x - v.x, y - v.y); }
    gk_vec2& operator-=(const gk_vec2 &v) { x -= v.x; y -= v.y; return *this; }

    gk_vec2 operator*(float f) const { return gk_vec2(x * f, y * f); }
#endif
} gk_vec2;

typedef struct GK_CXX_API gk_vec3 {
    float x, y, z;

    CXX_SET(float x, float y, float z);
    CXX_SET(float*);
} gk_vec3;

typedef struct GK_CXX_API gk_vec4 {
    float x, y, z, w;

    CXX_SET(float x, float y, float z, float w);
    CXX_SET(float*);
} gk_vec4;

typedef struct GK_CXX_API gk_mat4 {
    float a00, a01, a02, a03;
    float a10, a11, a12, a13;
    float a20, a21, a22, a23;
    float a30, a31, a32, a33;

    CXX_SET(float*);

#if defined(__cplusplus) && defined(GK_GLM_CONVERSIONS)
    inline operator glm::mat4&() { return (glm::mat4&)*this; }
#endif
} gk_mat4;

#endif  /* __GAMEKERNEL_MATH_H__ */
