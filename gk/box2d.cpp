#include <string.h>
#include <math.h>

#include <new>
#include <vector>
#include <gk/gk.hpp>
#include "gk/log.hpp"
#include "Box2D/Box2D.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include "nanovg.h"

class B2NvgDraw : public b2Draw {
    gk_context *_gk;
    int _w, _h;
    float _xscale, _yscale;
    float _linewidth;
public:
    B2NvgDraw(gk_context *gk) : _gk(gk), _w(0), _h(0) { }

    void setSize(int w, int h, float xscale, float yscale) {
        _w = w;
        _h = h;
        _xscale = xscale;
        _yscale = yscale;
        _linewidth = 2.0/xscale;
    }

    void DrawPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) {
        LOG("draw poly?");
    }

    void DrawSolidPolygon(const b2Vec2 *verts, int32 vertexCount, const b2Color &c) {
        auto nvg = _gk->nvg;

        nvgBeginPath(nvg);
        nvgMoveTo(nvg, verts[0].x, verts[0].y);
        for(int32 i = 1; i < vertexCount; ++i) {
            auto &vert = verts[i];
            nvgLineTo(nvg, vert.x, vert.y);
        }
        nvgClosePath(nvg);
        nvgFillColor(nvg, nvgRGBAf(c.r, c.g, c.b, c.a/2));
        nvgStrokeColor(nvg, nvgRGBAf(c.r, c.g, c.b, 1.0));
        nvgStrokeWidth(nvg, _linewidth);
        nvgFill(nvg);
        nvgStroke(nvg);
    }

    void DrawCircle(const b2Vec2 &center, float32 radius, const b2Color &c) {
        auto nvg = _gk->nvg;

        nvgBeginPath(nvg);
        nvgCircle(nvg, center.x, center.y, radius);
        nvgStrokeColor(nvg, nvgRGBAf(c.r, c.g, c.b, c.a));
        nvgStrokeWidth(nvg, _linewidth);
        nvgStroke(nvg);
    }

    void DrawSolidCircle(const b2Vec2 &center, float32 radius, const b2Vec2 &axis, const b2Color &c) {
        auto nvg = _gk->nvg;

        nvgBeginPath(nvg);
        nvgCircle(nvg, center.x, center.y, radius);
        nvgFillColor(nvg, nvgRGBAf(c.r, c.g, c.b, c.a/2));
        nvgStrokeColor(nvg, nvgRGBAf(c.r, c.g, c.b, c.a));
        nvgStrokeWidth(nvg, _linewidth);
        nvgFill(nvg);
        nvgStroke(nvg);

        nvgBeginPath(nvg);
        nvgMoveTo(nvg, center.x, center.y);
        nvgLineTo(nvg, center.x+(axis.x * radius), center.y+(axis.y * radius));
        nvgStroke(nvg);
    }

    void DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color) {
        LOG("draw segment");
    }

    void DrawTransform(const b2Transform &tf) {
        LOG("Draw Transform ", tf.p.x, ",", tf.p.y, " ∠ ", tf.q.GetAngle());
    }
};

void gk_process_b2_world_create(gk_context *gk, gk_cmd_b2_world_create *cmd) {
    b2Vec2 gravity(cmd->gravity.x, cmd->gravity.y);
    LOG("gravity = ", gravity.x, ",", gravity.y);
    auto world = new b2World(gravity);
    auto draw = new B2NvgDraw(gk);

    draw->SetFlags(b2Draw::e_shapeBit);

    world->SetAllowSleeping(cmd->do_sleep);
    world->SetDebugDraw(draw);

    cmd->world->data = (void*)world;
    cmd->world->draw = (void*)draw;
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
        def.allowSleep       = src.allow_sleep;
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
        src.body->data = body;
    }
}

void gk_process_b2_fixture_create(gk_context *gk, gk_cmd_b2_fixture_create *cmd) {
    std::vector<gk_vec2> verts;
    auto body = (b2Body*)cmd->body->data;

    b2FixtureDef fixdef;

    b2Vec2 p0;
    b2PolygonShape poly;
    b2CircleShape circle;
    b2ChainShape chain;

    auto def = cmd->pathdef;
    auto end = def + cmd->pathlen;

    for(; def < end; ++def) {
        int id = (int)def[0];

        switch(id) {
            case GK_PATH_RECT: {
                float w = def[3]/2, h = def[4]/2;
                b2Vec2 center(def[1] + w, def[2] + h);
                poly.SetAsBox(w, h, center, 0.0);
                fixdef.shape = &poly;
            }
                def += 4;
                break;

            case GK_PATH_CIRCLE:
                circle.m_p = b2Vec2(def[1], def[2]);
                circle.m_radius = def[3];
                fixdef.shape = &circle;
                def += 4;
                break;   

            case GK_PATH_DENSITY:
                fixdef.density = def[1];
                def++;
                break;

            case GK_PATH_ELASTICITY:
                fixdef.restitution = def[1];
                def++;
                break;

            case GK_PATH_FRICTION:
                fixdef.friction = def[1];
                def++;
                break;

            case GK_PATH_CATEGORY:
                fixdef.filter.categoryBits = def[1];
                def++;
                break;

            case GK_PATH_CATEGORY_MASK:
                fixdef.filter.maskBits = def[1];
                def++;
                break;

            case GK_PATH_GROUP:
                fixdef.filter.groupIndex = def[1];
                def++;
                break;

            case GK_PATH_FILL:
                if(fixdef.shape)
                    body->CreateFixture(&fixdef);

                break;

            case GK_PATH_BEGIN:
                new (&fixdef) b2FixtureDef;
                break;

            // Don't worry about unusable bits for now on the off chance
            // reusing paths for drawing and physics is useful.
            default: break;
        }
    }
}

void gk_process_b2_draw_debug(gk_context *gk, gk_cmd_b2_draw_debug *cmd) {
    auto world = (b2World*)cmd->world->data;
    auto draw = (B2NvgDraw*)cmd->world->draw;

    draw->setSize(cmd->width, cmd->height, 100.0, 100.0);

    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(gk->nvg, cmd->width, cmd->height, 1.0);
    nvgTransform(gk->nvg, 1, 0, 0, -1, 0, cmd->height);
    
    nvgScale(gk->nvg, 100, 100);
    world->DrawDebugData();
    nvgEndFrame(gk->nvg);
}

void gk_process_b2_step(gk_context *gk, gk_cmd_b2_step *cmd) {
    auto world = (b2World*)cmd->world->data;
    world->Step(1/60.0, 8, 3);
}

void gk_process_box2d(gk_context *gk, gk_bundle *bundle, gk_list *list) {
    for(int j = 0; j < list->ncmds; ++j) {
        auto cmd = list->cmds[j];
        auto type = GK_CMD_TYPE(cmd);

        switch(type) {
            case GK_CMD_B2_BODY_CREATE:
                gk_process_b2_body_create(gk, (gk_cmd_b2_body_create*)cmd);
                break;

            case GK_CMD_B2_FIXTURE_CREATE:
                gk_process_b2_fixture_create(gk, (gk_cmd_b2_fixture_create*)cmd);
                break;

            case GK_CMD_B2_DRAW_DEBUG:
                gk_process_b2_draw_debug(gk, (gk_cmd_b2_draw_debug*)cmd);
                break;

            case GK_CMD_B2_STEP:
                gk_process_b2_step(gk, (gk_cmd_b2_step*)cmd);
                break;

            default:
                gk_process_cmd_general("GK_LIST_BOX2D", gk, bundle, cmd);
                break;
        }
    }
}
