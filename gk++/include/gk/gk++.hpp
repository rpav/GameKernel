#pragma once

/*
  This is a _thin_ wrapper of GameKernel stuff for considerably-
  more-convenient use in C++.  Using GK from C directly is not
  intended.

  This uses a slight bit of STL.  You can probably replace this with
  a slimmer implementation without issues.
 */

#include <vector>

#include "gk/gk.h"

namespace gk {
    void initBundle(gk_bundle *b, unsigned int start = 0,
                    gk_pass_sorting sort = GK_PASS_SORT_NONE);
    void initList(gk_list *l, gk_subsystem sys);

    void initCmdPath(gk_cmd_path *cmd, float *pathdef = nullptr);
    
}
