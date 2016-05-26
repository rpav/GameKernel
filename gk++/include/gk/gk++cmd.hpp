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

        T cmd;
    };

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
}
