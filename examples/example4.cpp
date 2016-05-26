#include <GL/glew.h>
#include <GL/gl.h>
#include "gk/gk++.hpp"
#include "gk/log.hpp"
#include "example.hpp"

void example_main() {
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    auto gk = gk::create(GK_GL3);

    gk::Bundle bundle;
    gk::ListNvg nvg(1280, 720, 1.0);

    gk::CmdPath path;

    gk::PathDef pathdef = {
        GK_PATH_BEGIN,
        GK_PATH_RECT, 100, 100, 120, 30,
        GK_PATH_CIRCLE, 120, 120, 5,
        GK_PATH_WINDING, GK_WINDING_HOLE,
        GK_PATH_FILL_COLOR_RGBA, 255, 192, 0, 255,
        GK_PATH_FILL,
        GK_PATH_FILL_COLOR_RGBA, 0, 0, 255, 255
    };

    bundle.add(nvg);
    nvg.add(path);

    path.setPath(pathdef);

    gk::process(gk, bundle);
    swap();

    wait();
    gk::destroy(gk);
}
