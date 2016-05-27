#include <GL/glew.h>
#include <GL/gl.h>
#include "gk/gk++.hpp"
#include "gk/log.hpp"
#include "example.hpp"

void example_main() {
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    auto gk = gk::create(GK_GL3);

    gk::Bundle bundle(0);
    gk::List passes;
    gk::CmdPass pass_config(1);
    gk::CmdPass pass_draw(2);

    gk::List config;

    gk_b2d_world world;
    gk::vec2 gravity(0, -9.8);
    gk::CmdB2DWorldCreate createWorld(world, gravity, true);
    gk::CmdB2DWorldDestroy destroyWorld(world);

    bundle.add(passes,
               config);

    passes.add(pass_config);

    config.add(createWorld,
               destroyWorld);

    gk::process(gk, bundle);
    swap();

    wait();
    gk::destroy(gk);
}
