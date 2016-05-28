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

    gk::ListB2 physConfig;

    gk::B2World world;

    gk::B2Body body;
    gk::B2BodyDef bdef(body);

    gk::vec2 gravity(0, -9.8);
    gk::CmdB2WorldCreate createWorld(world, gravity, true);
    gk::CmdB2WorldDestroy destroyWorld(world);
    gk::CmdB2BodyCreate bodyCreate(world);

    bundle.add(passes,
               physConfig);

    passes.add(pass_config);

    physConfig.add(createWorld,
                   bodyCreate,
                   destroyWorld);

    bodyCreate.add(bdef);

    gk::process(gk, bundle);
    swap();

    wait();
    gk::destroy(gk);
}
