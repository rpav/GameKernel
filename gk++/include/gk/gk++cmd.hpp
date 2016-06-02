#pragma once

#include "gk/log.hpp"
#include <string.h>

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
            cmd.dsbuffer = create.cmd.dsbuffer;
            cmd.tex = create.cmd.tex;
        }
    };

    // gk::CmdRtUnbind
    class CmdRtUnbind : public CmdTmpl<gk_cmd_rt_unbind, GK_CMD_RT_UNBIND> {
    public:
        CmdRtUnbind() : CmdTmpl() { }
    };
}
