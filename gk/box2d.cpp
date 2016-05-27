#include <gk/gk.hpp>
#include "Box2D/Box2D.h"

void gk_process_b2_world_create(gk_context *gk, gk_cmd_b2_world_create *cmd) {
    b2Vec2 gravity(0,0);
    auto world = new b2World(gravity);
    world->SetAllowSleeping(cmd->do_sleep);

    cmd->world->data = (void*)world;
}

void gk_process_b2_world_destroy(gk_context *gk, gk_cmd_b2_world_destroy *cmd) {
    b2World *world = (b2World*)cmd->world->data;
    delete world;
    cmd->world->data = nullptr;
}

void gk_process_b2_body_create(gk_context *gk, gk_cmd_b2_body_create *cmd) {
    auto world = (b2World*)cmd->world->data;

    for(int i = 0; i < cmd->ndefs; ++i) {
        b2BodyDef def;
        auto &src = *(cmd->defs[i]);

        def.active           = src.active;
        def.allowSleep       = !src.no_sleep;
        def.angle            = src.angle;
        def.angularDamping   = src.angular_damping;
        def.angularVelocity  = src.angular_velocity;
        def.awake            = src.awake;
        def.bullet           = src.bullet;
        def.fixedRotation    = src.fixed_rotation;
        def.gravityScale     = src.gravity_scale;
        def.linearDamping    = src.linear_damping;
        def.linearVelocity.x = src.linear_velocity.x;
        def.linearVelocity.y = src.linear_velocity.y;
        def.position.x       = src.position.x;
        def.position.y       = src.position.y;
        def.type             = (b2BodyType)src.type;
        def.userData         = nullptr;

        auto body = world->CreateBody(&def);
    }
}

void gk_process_box2d(gk_context *gk, gk_bundle *bundle, gk_list *list) {
    for(int j = 0; j < list->ncmds; ++j) {
        auto cmd = list->cmds[j];
        auto type = GK_CMD_TYPE(cmd);

        switch(type) {
            case GK_CMD_B2_BODY_CREATE:
                gk_process_b2_body_create(gk, (gk_cmd_b2_body_create*)cmd);
                break;

            default:
                gk_process_cmd_general("GK_LIST_BOX2D", gk, bundle, cmd);
                break;
        }
    }
}
