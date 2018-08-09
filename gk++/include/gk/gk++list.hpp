#pragma once

#include <cstring>
#include <vector>

#include "gk/gk.h"

namespace gk {
    typedef std::vector<gk_list*> ListVector;
    typedef std::vector<gk_cmd*> CmdVector;

    class ListBase;
    
    class CmdBase {
    public:
        virtual ~CmdBase() {}
        virtual gk_cmd* cmdPtr() = 0;
    };

    class MultiCmd {
    public:
        virtual void addToList(ListBase&) = 0;
    };

    class ListBase {
    protected:
        CmdVector cmds;

    public:
        virtual ~ListBase() { }
        virtual gk_list* listPtr() = 0;

        void addCmd(CmdBase &cmd) {
            cmds.push_back(cmd.cmdPtr());
        }

        void addCmd(MultiCmd &cmds) {
            cmds.addToList(*this);
        }

        void updateList() {
            auto l = listPtr();
            l->ncmds = cmds.size();
            l->cmds = cmds.data();
        }
    };

    template <typename T, gk_subsystem SYS>
    class ListTmpl : public ListBase {

    public:
        T list;

        ListTmpl() {
            memset(&list, 0, sizeof(T));
            gk_list *l = (gk_list*)&list;
            l->sub = SYS;
        }
        virtual ~ListTmpl() { }
        virtual gk_list* listPtr() { return (gk_list*)&list; }

        template<typename...Rest>
        inline void add(CmdBase &cmd, Rest&...args) {
            addCmd(cmd);
            add(args...);
        }

        template<typename...Rest>
        inline void add(MultiCmd &cmds, Rest&...args) {
            addCmd(cmds);
            add(args...);
        }

        inline void add() { updateList(); }

        inline void clear() {
            gk_list *l = listPtr();
            cmds.clear();
            l->ncmds = 0;
            l->cmds = nullptr;
        }

        inline void reserve(CmdVector::size_type size) {
            cmds.reserve(size);
        }
    };

    class List : public ListTmpl<gk_list, GK_SUB_CONFIG> {
    public:
        List() : ListTmpl() { }
        virtual ~List() { }
    };

    class ListGL : public ListTmpl<gk_list_gl, GK_SUB_GL> {
    public:
        ListGL(float w = 0, float h = 0) : ListTmpl() {
            list.width = w;
            list.height = h;
        }
        virtual ~ListGL() { }
    };

    class ListNvg : public ListTmpl<gk_list_nvg, GK_SUB_NVG> {
    public:
        ListNvg() : ListTmpl() { }
        ListNvg(unsigned int width, unsigned int height, float ratio = 1.0, gk_origin origin = GK_ORIGIN_Y_DOWN)
            : ListTmpl()
        {
            list.width = width;
            list.height = height;
            list.ratio = ratio;
            list.origin = origin;
        }
        virtual ~ListNvg() { }

        void width(unsigned int w) { list.width = w; }
        void height(unsigned int h) { list.height = h; }
        void ratio(float r) { list.ratio = r; }
        void origin(gk_origin o) { list.origin = o; }
    };

    class ListB2 : public ListTmpl<gk_list, GK_SUB_BOX2D> {
    public:
        ListB2() : ListTmpl() { }
    };
}
