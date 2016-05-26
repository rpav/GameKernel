#include <string.h>

#include "gk/gk++.hpp"

BEGIN_NS_GK;

Bundle::Bundle(unsigned int start, gk_pass_sorting sort) {
    memset(&bundle, 0, sizeof(gk_bundle));

    bundle.start.parent.type = GK_CMD_PASS;
    bundle.start.sort = sort;
    bundle.start.list_index = start;
}


END_NS_GK;
