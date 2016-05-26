#pragma once

/*
  This uses a slight bit of STL.  You can probably replace this with
  a slimmer implementation without issues.
 */

#include <vector>
#include "gk/gk.h"
#include "gk/gk++cmd.hpp"
#include "gk/gk++list.hpp"

namespace gk {
    class Bundle {
        ListVector lists;
    public:
        Bundle(unsigned int start = 0, gk_pass_sorting sort = GK_PASS_SORT_NONE);
        ~Bundle() { }

        gk_bundle bundle;

        template<typename...Rest>
        void add(ListBase &list, Rest...args) {
            lists.push_back(list.listPtr());
            add(args...);
        }

        void add() {
            bundle.nlists = lists.size();
            bundle.lists = lists.data();
        }
    };

    inline gk_context* create(gk_impl impl) { return gk_create(impl); }
    inline void destroy(gk_context *gk) { gk_destroy(gk); }
    inline void process(gk_context *gk, Bundle &bundle) { gk_process(gk, &bundle.bundle); }
}

#define BEGIN_NS_GK namespace gk {
#define END_NS_GK }


