#pragma once

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

    /* gk::CmdB2DWorldCreate */
    class CmdB2DWorldCreate : public CmdTmpl<gk_cmd_b2d_world_create, GK_CMD_B2D_WORLD_CREATE> {
    public:
        CmdB2DWorldCreate(gk_b2d_world &world, float gravity, bool doSleep)
            : CmdTmpl() {
            cmd.world = &world;
            cmd.gravity = gravity;
            cmd.do_sleep = (char)doSleep;
        }
    };

    /* gk::CmdB2DWorldDestroy */
    class CmdB2DWorldDestroy : public CmdTmpl<gk_cmd_b2d_world_create, GK_CMD_B2D_WORLD_CREATE> {
    public:
        CmdB2DWorldDestroy(gk_b2d_world &world) : CmdTmpl() {
            cmd.world = &world;
        }
    };
}
