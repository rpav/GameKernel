#pragma once

/*
  This uses a slight bit of STL.  You can probably replace this with
  a slimmer implementation without issues.

  This also abuses templates and virtuals a bit to keep code concise.
 */

#include <vector>
#include "gk/gk.h"
#include "gk/gk++util.hpp"
#include "gk/gk++cmd.hpp"
#include "gk/gk++list.hpp"

namespace gk {
    class Bundle {
        ListVector lists;
    public:
        Bundle(unsigned int start = 0, gk_pass_sorting sort = GK_PASS_SORT_NONE);
        ~Bundle() { }
        void handleError();

        gk_bundle bundle;

        template<typename...Rest>
        inline void add(ListBase &list, Rest&...args) {
            lists.push_back(list.listPtr());
            add(args...);
        }

        inline void add() {
            bundle.nlists = lists.size();
            bundle.lists = lists.data();
        }

        inline void clear() {
            lists.clear();
            bundle.nlists = 0;
            bundle.lists = nullptr;
        }
    };

    typedef gk_context Context;

    inline Context* create(gk_impl impl) { return gk_create(impl); }
    inline void destroy(Context *gk) { gk_destroy(gk); }
    inline void process(Context *gk, Bundle &bundle) {
        gk_process(gk, &bundle.bundle);
        if(bundle.bundle.error.code)
            bundle.handleError();
    }
}

#define BEGIN_NS_GK namespace gk {
#define END_NS_GK }
