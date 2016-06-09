#pragma once

#include "gk/log.hpp"
#include <string.h>

#include <vector>
#include <map>

namespace gk {
    typedef std::vector<gk_cmd*> CmdVector;

    class CmdBase {
    public:
        virtual ~CmdBase() { }
        virtual gk_cmd* cmdPtr() = 0;
    };

    template <typename T, gk_cmd_type ID>
    class CmdTmpl : public CmdBase {
    public:
        CmdTmpl(int key = 0) {
            memset(&cmd, 0, sizeof(T));
            gk_cmd *c = (gk_cmd*)&cmd;
            c->type = ID;
            c->key = key;
        }
        virtual ~CmdTmpl() { }
        virtual gk_cmd* cmdPtr() { return (gk_cmd*)&cmd; }

        void key(int key) { cmdPtr()->key = key; }

        T cmd;
    };

    /* gk::CmdPass */
    class CmdPass : public CmdTmpl<gk_pass, GK_CMD_PASS> {
    public:
        CmdPass(unsigned int index, gk_pass_sorting sort = GK_PASS_SORT_NONE)
            : CmdTmpl() {
            cmd.list_index = index;
            cmd.sort = sort;
        }

        inline void index(unsigned int index) { cmd.list_index = index; }
        inline void sort(gk_pass_sorting sort) { cmd.sort = sort; }
    };

    // gk::CmdClear
    class CmdClear : public CmdTmpl<gk_cmd_clear, GK_CMD_CLEAR> {
    public:
        CmdClear(uint32_t flags) : CmdTmpl() {
            cmd.flags = flags;
            cmd.depth = 1.0;
        }

        CmdClear(float r, float g, float b, float a = 1.0)
            : CmdClear(GK_CLEAR_COLOR) {
            cmd.color.x = r;
            cmd.color.y = g;
            cmd.color.z = b;
            cmd.color.w = a;
        }
    };

    // gk::CmdQuad
    class CmdQuad : public CmdTmpl<gk_cmd_quad, GK_CMD_QUAD> {
    public:
        CmdQuad(int tex = 0) : CmdTmpl() {
            cmd.tex = tex;
            cmd.tfm = gk::mat4(0);
        }

        // For trivial quads
        CmdQuad(int tex, float x0, float y0, float x1, float y1)
            : CmdQuad(tex) {
            setVertex(0, x0, y0);
            setVertex(1, x1, y0);
            setVertex(2, x0, y1);
            setVertex(3, x1, y1);

            setUV(0, 0, 0);
            setUV(1, 1, 0);
            setUV(2, 0, 1);
            setUV(3, 1, 1);
        }

        inline void setVertex(int n, gk_vec4 &v) {
            cmd.attr[n].vertex = v;
        }

        inline void setVertex(int n, float x, float y, float z = 0.0, float w = 1.0) {
            cmd.attr[n].vertex.x = x;
            cmd.attr[n].vertex.y = y;
            cmd.attr[n].vertex.z = z;
            cmd.attr[n].vertex.w = w;
        }

        inline void setUV(int n, gk_vec2 &v) {
            cmd.attr[n].uv = v;
        }

        inline void setUV(int n, float u, float v) {
            cmd.attr[n].uv.x = u;
            cmd.attr[n].uv.y = v;
        }
    };

    /* gk::CmdPath */

    typedef std::vector<float> PathDef;

    class CmdPath : public CmdTmpl<gk_cmd_path, GK_CMD_PATH> {
    public:
        CmdPath() : CmdTmpl() { }
        virtual ~CmdPath() { }

        void setPath(float *pathdef, size_t count) {
            cmd.pathdef = pathdef;
            cmd.pathlen = count;
        }

        void setPath(PathDef &pathdef) {
            cmd.pathdef = pathdef.data();
            cmd.pathlen = pathdef.size();
        }
    };

    // gk::CmdFontCreate
    class CmdFontCreate : public CmdTmpl<gk_cmd_font_create, GK_CMD_FONT_CREATE> {
    public:
        CmdFontCreate(const char *name, const char *filename)
            : CmdTmpl() {
            cmd.name = name;
            cmd.filename = filename;
        }
    };

    // gk::CmdFontStyle
    class CmdFontStyle : public CmdTmpl<gk_cmd_font_style, GK_CMD_FONT_STYLE> {
    public:
        CmdFontStyle(float size = 0.0) : CmdTmpl() {
            cmd.size = size;
            cmd.spacing = 1.0;
            cmd.line_height = 1.0;
            cmd.align = GK_ALIGN_LEFT | GK_ALIGN_BASELINE;
        }
    };

    // gk::CmdText
    class CmdText : public CmdTmpl<gk_cmd_text, GK_CMD_TEXT> {
    public:
        CmdText(const char *string = nullptr, float x = 0.0, float y = 0.0) : CmdTmpl() {
            cmd.str = string;
            cmd.pos.x = x;
            cmd.pos.y = y;
        }

        CmdText(const char *string, gk_vec2 &pos) {
            cmd.str = string;
            cmd.pos = pos;
        }
    };

    // gk::B2World
    struct B2World : public gk_b2_world {
        B2World() { memset(this, 0, sizeof(*this)); }
    };

    /* gk::CmdB2WorldCreate */
    class CmdB2WorldCreate : public CmdTmpl<gk_cmd_b2_world_create, GK_CMD_B2_WORLD_CREATE> {
    public:
        CmdB2WorldCreate(gk_b2_world &world, vec2 &gravity, bool doSleep)
            : CmdTmpl() {
            cmd.world = &world;
            cmd.gravity = gravity;
            cmd.do_sleep = (char)doSleep;
        }
    };

    /* gk::CmdB2WorldDestroy */
    class CmdB2WorldDestroy : public CmdTmpl<gk_cmd_b2_world_destroy, GK_CMD_B2_WORLD_DESTROY> {
    public:
        CmdB2WorldDestroy(gk_b2_world &world) : CmdTmpl() {
            cmd.world = &world;
        }
    };

    // gk::B2Body
    struct B2Body : public gk_b2_body {
        B2Body() { memset(this, 0, sizeof(*this)); }

        B2Body(gk_vec2 &pos_, float &angle_)
            : B2Body()
        {
            pos = &pos_;
            angle = &angle_;
        }
    };

    struct B2BodyDef : public gk_b2_bodydef {
        B2BodyDef(B2Body &b, gk_b2_body_type type = GK_B2_BODY_TYPE_STATIC) {
            memset(this, 0, sizeof(*this));
            this->body = &b;
            this->type = type;
            this->active = true;
            this->awake = true;
            this->gravity_scale = 1.0;
            this->allow_sleep = true;
        }
    };

    // gk::CmdB2BodyCreate
    typedef std::vector<gk_b2_bodydef*> BodydefVector;
    class CmdB2BodyCreate : public CmdTmpl<gk_cmd_b2_body_create, GK_CMD_B2_BODY_CREATE> {
        BodydefVector defs;

    public:
        CmdB2BodyCreate(gk_b2_world &world)
            : CmdTmpl() {
            cmd.world = &world;
        }

        template<typename...Rest>
        inline void add(B2BodyDef &bd, Rest&...args) {
            defs.push_back(&bd);
            add(args...);
        }

        inline void add() {
            cmd.ndefs = defs.size();
            cmd.defs = defs.data();
        }
    };

    // gk::CmdB2FixtureCreate
    class CmdB2FixtureCreate : public CmdTmpl<gk_cmd_b2_fixture_create, GK_CMD_B2_FIXTURE_CREATE> {
    public:
        CmdB2FixtureCreate(gk_b2_body &body)
            : CmdTmpl() {
            cmd.body = &body;
        }

        void setPath(float *pathdef, size_t count) {
            cmd.pathdef = pathdef;
            cmd.pathlen = count;
        }

        void setPath(PathDef &pathdef) {
            cmd.pathdef = pathdef.data();
            cmd.pathlen = pathdef.size();
        }
    };

    // gk::CmdB2Step
    class CmdB2Step : public CmdTmpl<gk_cmd_b2_step, GK_CMD_B2_STEP> {
    public:
        CmdB2Step(gk_b2_world &world)
            : CmdTmpl() {
            cmd.world = &world;
        }
    };

    class CmdB2IterBodies : public CmdTmpl<gk_cmd_b2_iter_bodies, GK_CMD_B2_ITER_BODIES> {
    public:
        CmdB2IterBodies(gk_b2_world &world)
            : CmdTmpl() {
            cmd.world = &world;
        }
    };

    // gk::CmdB2DrawDebug
    class CmdB2DrawDebug : public CmdTmpl<gk_cmd_b2_draw_debug, GK_CMD_B2_DRAW_DEBUG> {
    public:
        CmdB2DrawDebug(gk_b2_world &world, int width, int height)
            : CmdTmpl() {
            cmd.world = &world;
            cmd.width = width;
            cmd.height = height;
        }
    };

    // gk::CmdRtCreate
    class CmdRtCreate :  public CmdTmpl<gk_cmd_rt_create, GK_CMD_RT_CREATE> {
    public:
        CmdRtCreate(uint32_t width, uint32_t height, uint32_t flags = GK_RT_ALPHA | GK_RT_STENCIL)
            : CmdTmpl() {
            cmd.width = width;
            cmd.height = height;

            cmd.rt_flags = flags;

            cmd.tex_min_filter = GK_TEX_FILTER_LINEAR;
            cmd.tex_mag_filter = GK_TEX_FILTER_LINEAR;
        }
    };

    // gk::CmdRtDestroy
    class CmdRtDestroy : public CmdTmpl<gk_cmd_rt_destroy, GK_CMD_RT_DESTROY> {
    public:
        CmdRtDestroy() : CmdTmpl() { }
        CmdRtDestroy(CmdRtCreate &create) {
            cmd.framebuffer = create.cmd.framebuffer;
            cmd.dsbuffer    = create.cmd.dsbuffer;
            cmd.tex         = create.cmd.tex;
        }
    };

    // gk::CmdRtBind
    class CmdRtBind : public CmdTmpl<gk_cmd_rt_bind, GK_CMD_RT_BIND> {
    public:
        CmdRtBind() : CmdTmpl() { }
        CmdRtBind(uint32_t framebuffer) : CmdTmpl() {
            cmd.framebuffer = framebuffer;
        }
        CmdRtBind(CmdRtCreate &create) : CmdTmpl() {
            cmd.framebuffer = create.cmd.framebuffer;
        }
    };

    // gk::CmdRtUnbind
    class CmdRtUnbind : public CmdTmpl<gk_cmd_rt_unbind, GK_CMD_RT_UNBIND> {
    public:
        CmdRtUnbind() : CmdTmpl() { }
    };

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

    // gk::CmdProgramCreate
    typedef std::vector<gk_program_source*> ProgramSourceVector;
    class CmdProgramCreate : public CmdTmpl<gk_cmd_program_create, GK_CMD_PROGRAM_CREATE> {
        ProgramSourceVector _sources;

    public:
        CmdProgramCreate() : CmdTmpl() { }

        template<typename...Rest>
        inline void add(ProgramSource &bd, Rest&...args) {
            _sources.push_back(&bd.source);
            add(args...);
        }

        inline void add() {
            cmd.nsources = _sources.size();
            cmd.source   = _sources.data();
        }
    };

    // gk::CmdProgramDestroy
    typedef std::vector<gk_program> ProgramVector;
    class CmdProgramDestroy : public CmdTmpl<gk_cmd_program_destroy, GK_CMD_PROGRAM_DESTROY> {
        ProgramVector _progs;

    public:
        CmdProgramDestroy() : CmdTmpl() { }

        template<typename...Rest>
        inline void add(gk_program &p, Rest&...args) {
            _progs.push_back(p);
            add(args...);
        }

        template<typename...Rest>
        inline void add(CmdProgramCreate &cmd, Rest&...args) {
            auto &c = cmd.cmd;
            for(int i = 0; i < c.nsources; ++i) {
                _progs.push_back(c.source[i]->program);
            }
            add(args...);
        }

        inline void add() {
            cmd.nprograms = _progs.size();
            cmd.program   = _progs.data();
        }
    };

    // gk::CmdUniformQuery
    typedef std::vector<const char *> NameVector;
    typedef std::vector<gk_uniform> UniformVector;

    struct strcmp_f { bool operator()(const char *a, const char *b) { return strcmp(a,b) < 0; } };
    typedef std::map<const char *, gk_uniform, strcmp_f> UniformNameLocationMap;

    class CmdUniformQuery : public CmdTmpl<gk_cmd_uniform_query, GK_CMD_UNIFORM_QUERY> {
        NameVector _names;
        UniformVector _uniforms;
        UniformNameLocationMap _locations;

    public:
        CmdUniformQuery(gk_program &program) {
            cmd.program = &program;
        }
        CmdUniformQuery(ProgramSource &source) : CmdUniformQuery(source.source.program) { }

        template<typename...Rest>
        inline void add(const char *name, Rest...args) {
            _names.push_back(name);
            add(args...);
        }

        inline void add() {
            cmd.nuniforms = _names.size();
            cmd.names     = _names.data();

            _uniforms.reserve(_names.size());
            cmd.uniforms = _uniforms.data();
        }

        void index() {
            _locations.clear();
            for(size_t i = 0; i < cmd.nuniforms; ++i) {
                _locations[cmd.names[i]] = cmd.uniforms[i];
            }
        }

        gk_uniform find(const char *name) {
            auto i = _locations.find(name);

            if(i != _locations.end())
                return i->second;

            return -1;
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

        inline size_t add(gk_uniform location, double v) {
            _values.emplace_back(location, (float)v);
            return update();
        }

        inline size_t add(gk_uniform location, float v) {
            _values.emplace_back(location, v);
            return update();
        }

        inline size_t add(gk_uniform location, int v) {
            _values.emplace_back(location, v);
            return update();
        }

        template<typename...Rest>
        inline size_t add(gk_uniform location, unsigned int v) {
            _values.emplace_back(location, v);
            return update();
        }
    };

    // gk::ProgramDataSet
    class ProgramDataSet {
    public:
        gk_program_data_set pds;

        ProgramDataSet(gk_program program = 0) {
            memset(&pds, 0, sizeof(pds));
            pds.program = program;
        }

        ProgramDataSet(ProgramSource &program) : ProgramDataSet(program.source.program) { }

        void update(gk_program program) {
            pds.program = program;
            pds.dirty |= GK_PDS_DIRTY_PROGRAM;
        }

        void update(UniformSet &uniforms) {
            pds.uniforms = &uniforms.uniform_set;
            pds.dirty |= GK_PDS_DIRTY_UNIFORMS;
        }
    };
}
