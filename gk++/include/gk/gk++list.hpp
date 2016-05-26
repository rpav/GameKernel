#pragma once

#include <string.h>

namespace gk {
    typedef std::vector<gk_list*> ListVector;

    class ListBase {
    public:
        virtual ~ListBase() { }
        virtual gk_list* listPtr() = 0;
    };

    template <typename T, gk_subsystem SYS>
    class ListTmpl : public ListBase {
        CmdVector cmds;

    public:
        ListTmpl() {
            memset(&list, 0, sizeof(T));
            gk_list *l = (gk_list*)&list;
            l->sub = SYS;
        }
        virtual ~ListTmpl() { }
        virtual gk_list* listPtr() { return (gk_list*)&list; }

        T list;

        template<typename...Rest>
        inline void add(CmdBase &cmd, Rest...args) {
            cmds.push_back(cmd.cmdPtr());
            add(args...);
        }

        template<typename...Rest>
        inline void add(CmdBase *cmd, Rest...args) {
            cmds.push_back(cmd->cmdPtr());
            add(args...);
        }

        inline void add() {
            gk_list *l = listPtr();
            l->ncmds = cmds.size();
            l->cmds = cmds.data();
        }
    };

    class List : public ListTmpl<gk_list, GK_SUB_CONFIG> {
    public:
        List() : ListTmpl() { }
        virtual ~List() { }
    };

    class ListNvg : public ListTmpl<gk_list_nvg, GK_SUB_NVG> {
    public:
        ListNvg() : ListTmpl() { }
        ListNvg(unsigned int width, unsigned int height, float ratio = 1.0)
            : ListTmpl()
        {
            list.width = width;
            list.height = height;
            list.ratio = ratio;
        }
        virtual ~ListNvg() { }

        void width(unsigned int w) { list.width = w; }
        void height(unsigned int h) { list.height = h; }
        void ration(float r) { list.ratio = r; }
    };
}
