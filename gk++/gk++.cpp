#include <string.h>

#include "gk/gk++.hpp"
#include "gk/log.hpp"

BEGIN_NS_GK;

Bundle::Bundle(unsigned int start, gk_pass_sorting sort) {
    memset(&bundle, 0, sizeof(gk_bundle));

    bundle.start.parent.type = GK_CMD_PASS;
    bundle.start.sort = sort;
    bundle.start.list_index = start;
}

void Bundle::handleError() {
    if(!bundle.error.code) return;

    LOG("gk++: Error processing bundle (",
        bundle.error.code,
        "): ",
        bundle.error.message);
}

END_NS_GK;
