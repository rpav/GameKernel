#pragma once

/*
  This uses a slight bit of STL.  You can probably replace this with
  a slimmer implementation without issues.

  This also abuses templates and virtuals a bit to keep code concise.
 */

#include <stdexcept>
#include <vector>

#include <rpav/ptr.hpp>

#include "gk/gk++cmd.hpp"
#include "gk/gk++list.hpp"
#include "gk/gk++util.hpp"
#include "gk/gk.h"

namespace gk {

class Error : public std::runtime_error {
public:
    gk_error_code code{};

    Error(gk_error_code code, const char* msg) : runtime_error{msg}, code{code} {}
};

class Bundle {
    ListVector lists;

public:
    Bundle(unsigned int start = 0, gk_pass_sorting sort = GK_PASS_SORT_NONE);
    ~Bundle() {}
    void handleError();

    gk_bundle bundle;

    template<typename... Rest>
    inline void add(ListBase& list, Rest&... args)
    {
        lists.push_back(list.listPtr());
        add(args...);
    }

    inline void add()
    {
        bundle.nlists = lists.size();
        bundle.lists  = lists.data();
    }

    inline void clear()
    {
        lists.clear();
        bundle.nlists = 0;
        bundle.lists  = nullptr;
    }
};

struct Context {
    rpav::ptr<gk_context> ctx;

    Context(gk_impl impl) { ctx = gk_create(impl); }
    ~Context() { gk_destroy(ctx); }

    operator gk_context*() { return ctx; }
    operator const gk_context*() const { return ctx; }

    inline void process(Bundle& bundle)
    {
        gk_process(ctx, &bundle.bundle);
        if(bundle.bundle.error.code) bundle.handleError();
    }

    template<typename L, typename...Ts>
    inline void process(Ts&...vs) {
        gk::Bundle b;
        L list;
        b.add(list);
        list.add(vs...);
        process(b);
    }
};

} // namespace gk

#define BEGIN_NS_GK namespace gk {
#define END_NS_GK }
