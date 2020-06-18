#pragma once

#include <cstring>
#include <vector>

#include "gk/gk.h"

namespace gk {
using ListVector = std::vector<gk_list*>;
using CmdVector  = std::vector<gk_cmd*>;

class ListBase;

class CmdBase {
public:
    virtual ~CmdBase()       = default;
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
    virtual ~ListBase()        = default;
    virtual gk_list* listPtr() = 0;

    void addCmd(CmdBase& cmd) { cmds.push_back(cmd.cmdPtr()); }

    void addCmd(MultiCmd& cmds) { cmds.addToList(*this); }

    void updateList()
    {
        auto l   = listPtr();
        l->ncmds = cmds.size();
        l->cmds  = cmds.data();
    }
};

template<typename T, gk_subsystem SYS>
class ListTmpl : public ListBase {

public:
    T list;

    ListTmpl()
    {
        memset(&list, 0, sizeof(T));
        auto l = (gk_list*)&list;
        l->sub = SYS;
    }

    gk_list* listPtr() override { return (gk_list*)&list; }

    template<typename C, typename... Rest>
    inline void add(C& cmd, Rest&... args)
    {
        addCmd(cmd);
        add(args...);
    }

    inline void add() { updateList(); }

    inline void clear()
    {
        gk_list* l = listPtr();
        cmds.clear();
        l->ncmds = 0;
        l->cmds  = nullptr;
    }

    inline void reserve(CmdVector::size_type size) { cmds.reserve(size); }
};

class List : public ListTmpl<gk_list, GK_SUB_CONFIG> {
public:
    List() : ListTmpl() {}
};

class ListGL : public ListTmpl<gk_list_gl, GK_SUB_GL> {
public:
    ListGL(gk::irect viewport = {}) : ListTmpl()
    {
        list.viewport = viewport;
        list.ratio    = 1.0f;
        list.origin   = GK_ORIGIN_Y_UP;
    }

    void width(unsigned int w) { list.viewport.size.x = w; }
    void height(unsigned int h) { list.viewport.size.y = h; }
    void ratio(float r) { list.ratio = r; }
    void origin(gk_origin o) { list.origin = o; }
};

using ListNvg = ListGL;

class ListB2 : public ListTmpl<gk_list, GK_SUB_BOX2D> {
public:
    ListB2() : ListTmpl() {}
};
} // namespace gk
