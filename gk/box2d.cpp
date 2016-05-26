#include <gk/gk.hpp>
#include "Box2D/Box2D.h"

void gk_process_b2d_world_create(gk_context *gk, gk_cmd_b2d_world_create *cmd) {
    b2Vec2 gravity(0,0);
    auto world = new b2World(gravity);
    world->SetAllowSleeping(cmd->do_sleep);

    cmd->world->data = (void*)world;
}

void gk_process_b2d_world_destroy(gk_context *gk, gk_cmd_b2d_world_destroy *cmd) {
    b2World *world = (b2World*)cmd->world->data;
    delete world;
    cmd->world->data = nullptr;
}

