#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include "gk/gk++.hpp"
#include <rpav/log.hpp>
#include "example.hpp"

void example_main(int, const char**) {
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

    gk::B2Body body;
    body.position = b_pos;
    body.angle = b_angle;

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
        GK_PATH_FILL,

        GK_PATH_BEGIN,
        GK_PATH_CIRCLE, 1.5, 2.0, 2.0,
        GK_PATH_SENSOR,
        GK_PATH_FIXTURE_ID, 1.0,
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

    // Some starting force
    gk::CmdB2LinearImpulse force(body, gk::vec2(0,-10), gk::vec2(2, 4));

    // Step, draw, iterate
    gk::CmdB2Step step(world);
    gk::CmdB2DrawDebug ddraw(world, 1280, 720);
    gk::CmdB2IterBodies iterBodies(world);

    ddraw.cmd.scale = {64,64};

    // Bundle it all up
    bundle.add(phys);

    phys.add(createWorld,
             bodyCreate,
             fixCreate,
             fixGroundCreate,
             force);

    // Go
    gk::process(gk, bundle);

    // -- Simulate --
    phys.clear();
    phys.add(step, ddraw, iterBodies);

    int frame = 0;

    while(!check_input()) {
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        gk::process(gk, bundle);

        if(step.cmd.ncollisions > 0) {
            for(int i = 0; i < step.cmd.ncollisions; ++i) {
                auto c = step.cmd.collisions[i];
                say(frame, " collide ", c.a, "(", c.id_a, ") <. ", c.b, "(", c.id_b, ") with ",
                    c.contact);
            }
        }

        swap();
        ++frame;
    }

    phys.clear();
    phys.add(destroyWorld);
    gk::process(gk, bundle);

    gk::destroy(gk);
}
