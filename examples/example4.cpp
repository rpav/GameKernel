#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include "gk/gk++.hpp"
#include "gk/log.hpp"
#include "example.hpp"

void example_main() {
    // Don't want unbounded framerate (at least right now)
    checkrc(SDL_GL_SetSwapInterval(1));

    auto gk = gk::create(GK_GL3);

    gk::Bundle bundle(0);
    gk::ListB2 phys;

    // World
    gk::vec2 gravity(0, -9.8);
    gk::B2World world;
    gk::CmdB2WorldCreate createWorld(world, gravity, true);
    gk::CmdB2WorldDestroy destroyWorld(world);

    // Body
    gk::vec2 b_pos;
    float b_angle;

    gk::B2Body body(b_pos, b_angle);
    gk::B2BodyDef bdef(body, GK_B2_BODY_TYPE_DYNAMIC);
    gk::CmdB2BodyCreate bodyCreate(world);
    bdef.angle = 0.2;
    bodyCreate.add(bdef);

    // Fixture
    gk::CmdB2FixtureCreate fixCreate(body);
    gk::PathDef fixPath = {
        GK_PATH_BEGIN,
        GK_PATH_RECT, 1.5, 2.0, 1.0, 1.0,
        GK_PATH_DENSITY, 1.0,
        GK_PATH_ELASTICITY, 0.5,
        GK_PATH_FILL,

        GK_PATH_BEGIN,
        GK_PATH_CIRCLE, 1.5, 2.0, 0.5,
        GK_PATH_DENSITY, 0.5,
        GK_PATH_ELASTICITY, 0.5,
        GK_PATH_FILL
    };
    fixCreate.setPath(fixPath);

    // Ground
    gk::B2Body ground;
    gk::B2BodyDef bdef_ground(ground, GK_B2_BODY_TYPE_STATIC);
    bodyCreate.add(bdef_ground);

    // Ground fixture
    gk::CmdB2FixtureCreate fixGroundCreate(ground);
    gk::PathDef groundPath = {
        GK_PATH_BEGIN,
        GK_PATH_RECT, 0.0, 0.2, 4.0, 0.1,
        GK_PATH_FILL
    };
    fixGroundCreate.setPath(groundPath);

    // Step, draw, iterate
    gk::CmdB2Step step(world);
    gk::CmdB2DrawDebug ddraw(world, 1280, 720);
    gk::CmdB2IterBodies iterBodies(world);

    // Bundle it all up
    bundle.add(phys);

    phys.add(createWorld,
             bodyCreate,
             fixCreate,
             fixGroundCreate);

    // Go
    gk::process(gk, bundle);

    // -- Simulate --
    phys.clear();
    phys.add(step, ddraw, iterBodies);

    while(!check_input()) {
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        gk::process(gk, bundle);

        if(body.is_awake)
            LOG("pos = ", b_pos.x, ",", b_pos.y, " ∠ ", b_angle);

        swap();
    }

    phys.clear();
    phys.add(destroyWorld);
    gk::process(gk, bundle);

    gk::destroy(gk);
}
