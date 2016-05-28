#pragma once

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
    class CmdB2WorldDestroy : public CmdTmpl<gk_cmd_b2_world_create, GK_CMD_B2_WORLD_CREATE> {
    public:
        CmdB2WorldDestroy(gk_b2_world &world) : CmdTmpl() {
            cmd.world = &world;
        }
    };

    // gk::B2Body
    struct B2Body : public gk_b2_body {
        B2Body() { memset(this, 0, sizeof(*this)); }
    };

    struct B2BodyDef : public gk_b2_bodydef {
        B2BodyDef(B2Body &b) {
            memset(this, 0, sizeof(*this));
            this->body = &b;
        }
    };

    // gk::CmdB2BodyDef
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
}
