#pragma once

#include <cstring>
#include <cmath>

#include <map>
#include <string>

namespace gk {
    struct vec2 : public gk_vec2 {
        vec2() = default;
        vec2(int) { x = 0; y = 0; }
        vec2(float x_, float y_) { x = x_; y = y_; }
        vec2(const gk_vec2 &v) { x = v.x; y = v.y; }

        vec2 operator+(const gk_vec2 &v) const { return vec2(x + v.x, y + v.y); }
        vec2& operator+=(const gk_vec2 &v) { x += v.x; y += v.y; return *this; }

        vec2 operator-(const gk_vec2 &v) const { return vec2(x - v.x, y - v.y); }
        vec2& operator-=(const gk_vec2 &v) { x -= v.x; y -= v.y; return *this; }

        vec2 operator*(float f) const { return vec2(x * f, y * f); }
        vec2& operator*=(float f) { x *= f; y *= f; return *this; }

        // Note: These are simple but not terribly efficient

        // Requires a normalized vector
        float angle(const gk_vec2 &v) const { return atan2(v.y,v.x) - atan2(y,x); }

        float length() const {
            return sqrt((x*x)+(y*y));
        }

        gk_vec2& normalize() {
            float scale = 1.0 / length();
            
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
    };

    struct vec3 : public gk_vec3 {
        vec3() = default;
        vec3(int) { x = 0; y = 0; z = 0; }
        vec3(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }

        vec3 operator+(const gk_vec3 &v) { return vec3(x + v.x, y + v.y, z + v.z); }
        vec3& operator+=(const gk_vec3 &v) { 
            x += v.x; y += v.y; z += v.z;
            return *this;
        }

        vec3 operator-(const gk_vec3 &v) { return vec3(x - v.x, y - v.y, z - v.z); }
        vec3& operator-=(const gk_vec3 &v) { 
            x -= v.x; y -= v.y; z -= v.z;
            return *this;
        }

    };

    struct vec4 : public gk_vec4 {
        vec4() = default;
        vec4(int) { x = 0; y = 0; z = 0; w = 0; }
        vec4(float x_, float y_, float z_, float w_) {
            x = x_; y = y_; z = z_; w = w_;
        }

        vec4 operator+(const gk_vec4 &v) { return vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
        vec4& operator+=(const gk_vec4 &v) {
            x += v.x; y += v.y; z += v.z; w += v.w;
            return *this;
        }

        vec4 operator-(const gk_vec4 &v) { return vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
        vec4& operator-=(const gk_vec4 &v) {
            x -= v.x; y -= v.y; z -= v.z; w -= v.w;
            return *this;
        }
    };

    struct mat4 : public gk_mat4 {
        mat4() = default;
        mat4(int) {
            memset(this, 0, sizeof(mat4));
            a00 = a11 = a22 = a33 = 1;
        }
    };

    template<typename T>
    inline T* ptr(T &x) { return &x; }

    template<typename T>
    inline T* ptr(T *x) { return x; }

    template<typename T> struct base_type      { typedef T type; };
    template<typename T> struct base_type<T*>  { typedef T type; };
    template<typename T> struct base_type<T&>  { typedef T type; };
    template<typename T> struct base_type<T&&> { typedef T type; };

    // gk::ShaderSource
    class ShaderSource {
    public:
        ShaderSource(gk_shader_type type) {
            memset(&source, 0, sizeof(source));
            source.type = type;
        }
        ShaderSource(gk_shader_type type, const char *source_)
            : ShaderSource(type) {
            source.source = source_;
        }

        gk_shader_source source;
    };

    // gk::ProgramSource
    typedef std::vector<gk_shader_source*> ShaderSourceVector;
    class ProgramSource {
        ShaderSourceVector _shaders;
    public:
        gk_program_source source;

        template<typename...Rest>
        inline void add(ShaderSource &src, Rest&...args) {
            _shaders.push_back(&(src.source));
            add(args...);
        }

        template<typename...Rest>
        inline void add(gk_shader_source *src, Rest&...args) {
            _shaders.push_back(src);
            add(args...);
        }

        inline void add() {
            source.nsources = _shaders.size();
            source.source   = _shaders.data();
        }

        inline void clear() {
            _shaders.clear();
            source.nsources = 0;
            source.source   = nullptr;
        }

        inline operator gk_program() { return source.program; }
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
            : UniformValue(location_, GK_UNIFORM_VALUE_VEC2) {
            value.data = &v;
        }

        UniformValue(gk_uniform location_, gk_vec3 &v)
            : UniformValue(location_, GK_UNIFORM_VALUE_VEC3) {
            value.data = &v;
        }

        UniformValue(gk_uniform location_, gk_vec4 &v)
            : UniformValue(location_, GK_UNIFORM_VALUE_VEC4) {
            value.data = &v;
        }

        UniformValue(gk_uniform location_, gk_mat4 &v)
            : UniformValue(location_, GK_UNIFORM_VALUE_MAT4) {
            value.data = &v;
        }

        void set(float f) {
            type = GK_UNIFORM_VALUE_FLOAT;
            value.f = f;
        }

        void set(int i) {
            type = GK_UNIFORM_VALUE_INT;
            value.i = i;
        }

        void set(unsigned int ui) {
            type = GK_UNIFORM_VALUE_UINT;
            value.ui = ui;
        }

        void set(gk_vec2 &v) {
            type = GK_UNIFORM_VALUE_VEC2;
            value.data = &v;
        }

        void set(gk_vec3 &v) {
            type = GK_UNIFORM_VALUE_VEC3;
            value.data = &v;
        }

        void set(gk_vec4 &v) {
            type = GK_UNIFORM_VALUE_VEC3;
            value.data = &v;
        }

        void set(gk_mat4 &v) {
            type = GK_UNIFORM_VALUE_MAT4;
            value.data = &v;
        }
    };

    // gk::UniformSet
    typedef std::vector<UniformValue> UniformValueVector;

    class UniformSet {
        UniformValueVector _values;

        size_t update() {
            auto n = _values.size();

            uniform_set.nuniforms = n;
            uniform_set.values = _values.data();

            return n-1;
        }

    public:
        gk_uniform_set uniform_set;

        UniformSet() {
            memset(&uniform_set, 0, sizeof(uniform_set));
        }

        template <typename T>
        inline size_t add(gk_uniform location, T &v) {
            _values.emplace_back(location, v);
            return update();
        }

        template <typename T>
        inline size_t add(gk_uniform location, T v) {
            _values.emplace_back(location, v);
            return update();
        }

        inline size_t add(gk_uniform location, double v) {
            _values.emplace_back(location, (float)v);
            return update();
        }

        // This is _index_ NOT _uniform location_: add() the uniform
        // to this set and remember the index it returns for this.
        template <typename T>
        inline void set(size_t index, T &v) {
            _values[index].set(v);
        }

        // Disambiguation
        inline void set(size_t index, float v)        { _values[index].set(v); }
        inline void set(size_t index, double v)       { _values[index].set((float)v); }
        inline void set(size_t index, int v)          { _values[index].set(v); }
        inline void set(size_t index, unsigned int v) { _values[index].set(v); }
    };

    // gk::ProgramDataSet
    template<typename T, bool OWN>
    class ProgramDataSetTmpl {
    public:
        T pds;

        ProgramDataSetTmpl() {
            if(OWN) memset(ptr(pds), 0, sizeof(typename base_type<T>::type));
        }

        ProgramDataSetTmpl(gk_program program) : ProgramDataSetTmpl() {
            ptr(pds)->program = program;
        }

        ProgramDataSetTmpl(ProgramSource &program)
            : ProgramDataSetTmpl(program.source.program) { }

        void set(gk_program program) {
            ptr(pds)->program = program;
            ptr(pds)->dirty |= GK_PDS_DIRTY_PROGRAM;
        }

        void set(UniformSet &uniforms) {
            ptr(pds)->uniforms = &uniforms.uniform_set;
            ptr(pds)->dirty |= GK_PDS_DIRTY_UNIFORMS;
        }
    };

    typedef ProgramDataSetTmpl<gk_program_data_set, true> ProgramDataSet;
    typedef ProgramDataSetTmpl<gk_program_data_set*, false> ProgramDataSetRef;

    // gk::SpriteSheet
    class CmdSpriteSheetCreate;
    class CmdSpriteSheetDestroy;

    class SpriteSheet {
        std::map<std::string,int32_t> _name_map;

    public:
        gk_spritesheet *sheet;

        SpriteSheet(gk_spritesheet *sheet);
        SpriteSheet(CmdSpriteSheetCreate &cmd);

        operator gk_spritesheet*() { return sheet; }

        void rereadSheet();

        inline size_t lookup(const std::string &name, size_t errval = ~0) {
            auto v = _name_map.find(name);
            if(v == _name_map.end()) return errval;
            return v->second;
        }
    };

    class SpriteLayerConfig : public gk_spritelayer_config {
    public:
        SpriteLayerConfig() {
            sheet = nullptr;
            layer_size = { 0, 0 };
            sprite_size = { 0, 0 };
        }
    };

    class SpriteLayerRender : public gk_spritelayer_render {
    public:
        SpriteLayerRender() {
            pds = nullptr;
            tfm = mat4(0);
            bounds = { 0, 0, 0, 0 };
            flags = 0;
        }
    };
}
