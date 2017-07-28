#include <string.h>
#include <math.h>

#include <new>
#include <vector>
#include <set>
#include <utility>
#include <gk/gk.hpp>
#include "gk/log.hpp"
#include "Box2D/Box2D.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include "nanovg.h"

class GK_B2NvgDraw : public b2Draw {
    gk_context *_gk;
    int _w, _h;
    float _xscale, _yscale;
    float _linewidth;
public:
    GK_B2NvgDraw(gk_context *gk) : _gk(gk), _w(0), _h(0) { }

    void setSize(int w, int h, float xscale, float yscale) {
        _w = w;
        _h = h;
        _xscale = xscale;
        _yscale = yscale;
        _linewidth = 1.0/xscale;
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

    void DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &c) {
        auto nvg = _gk->nvg;
        nvgBeginPath(nvg);
        nvgMoveTo(nvg, p1.x, p1.y);
        nvgLineTo(nvg, p2.x, p2.y);
        nvgStrokeColor(nvg, nvgRGBAf(c.r, c.g, c.b, c.a));
        nvgStrokeWidth(nvg, _linewidth);
        nvgStroke(nvg);
    }

    void DrawTransform(const b2Transform &tf) {
        LOG("Draw Transform ", tf.p.x, ",", tf.p.y, " ∠ ", tf.q.GetAngle());
    }
};

typedef struct gk_b2_contact_pair ContactPair;
typedef std::vector<ContactPair*> GkContactPtrVector;

struct gk_b2_body_data {
    b2Body *body;
};

struct gk_b2_fixture_data {
    int id;
};

class GK_B2ContactListener : public b2ContactListener {
    GkContactPtrVector _pairs;

public:
    void begin() {
        _pairs.clear();
    }

    ContactPair& add(b2Contact *c) {
        b2WorldManifold manifold;
        c->GetWorldManifold(&manifold);

        auto fixA = c->GetFixtureA();
        auto fixB = c->GetFixtureB();

        auto a = (gk_b2_body*)fixA->GetBody()->GetUserData();
        auto b = (gk_b2_body*)fixB->GetBody()->GetUserData();

        gk_b2_fixture_data *fixDataA = (gk_b2_fixture_data*)fixA->GetUserData();
        gk_b2_fixture_data *fixDataB = (gk_b2_fixture_data*)fixB->GetUserData();

        int idA = 0, idB = 0;

        if(fixDataA) idA = fixDataA->id;
        if(fixDataB) idB = fixDataB->id;

        auto pair = new ContactPair(a, b, idA, idB, *(gk_vec2*)&manifold.normal);
        _pairs.push_back(pair);

        return *pair;
    }

    virtual void BeginContact(b2Contact *c) {
        auto &pair = add(c);
        pair.contact = 1;
    }
    virtual void EndContact(b2Contact *c) {
        auto &pair = add(c);
        pair.contact = -1;
    }
    virtual void PreSolve (b2Contact *contact, const b2Manifold *oldManifold) {

    }
    virtual void PostSolve (b2Contact *contact, const b2ContactImpulse *impulse) {

    }

    void finish(gk_cmd_b2_step *cmd) {
        cmd->collisions = _pairs.data();
        cmd->ncollisions = _pairs.size();
    }
};

struct gk_b2_world_data {
    b2World *world;
    GK_B2NvgDraw *draw;
    GK_B2ContactListener *listen;
};

void gk_process_b2_world_create(gk_context *gk, gk_cmd_b2_world_create *cmd) {
    b2Vec2 gravity(cmd->gravity.x, cmd->gravity.y);

    auto draw = new GK_B2NvgDraw(gk);
    draw->SetFlags(b2Draw::e_shapeBit);

    auto world = new b2World(gravity);
    world->SetAllowSleeping(cmd->do_sleep);
    world->SetDebugDraw(draw);

    auto listen = new GK_B2ContactListener;
    world->SetContactListener(listen);

    auto data = new gk_b2_world_data;
    data->world = world;
    data->draw = draw;
    data->listen = listen;

    cmd->world->data = data;
}

void gk_process_b2_world_destroy(gk_context *, gk_cmd_b2_world_destroy *cmd) {
    delete cmd->world->data->world;
    delete cmd->world->data->draw;
    delete cmd->world->data->listen;

    cmd->world->data = nullptr;
}

void gk_process_b2_body_create(gk_context *, gk_cmd_b2_body_create *cmd) {
    auto world = cmd->world->data->world;

    for(size_t i = 0; i < cmd->ndefs; ++i) {
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
        def.userData         = src.body;

        auto body = world->CreateBody(&def);
        auto bodyData = new gk_b2_body_data;

        bodyData->body = body;
        src.body->data = bodyData;
    }
}

void gk_process_b2_body_update(gk_context *, gk_cmd_b2_body_update *cmd) {
    auto body = cmd->body->data->body;
    body->SetTransform((b2Vec2&)cmd->translate, cmd->angle);
}

void gk_process_b2_body_destroy(gk_context *, gk_cmd_b2_body_destroy *cmd) {
    auto world = cmd->world->data->world;
    auto body = cmd->body->data->body;
    world->DestroyBody(body);
}

gk_b2_fixture_data* ensure_fixdata(b2FixtureDef *fixdef) {
    if(fixdef->userData) return (gk_b2_fixture_data*)fixdef->userData;

    auto data = new gk_b2_fixture_data;
    fixdef->userData = data;

    return data;
}

void gk_process_b2_fixture_create(gk_context *, gk_cmd_b2_fixture_create *cmd) {
    std::vector<b2Vec2> verts;
    auto body = cmd->body->data->body;
    float scale = (cmd->scale != 0.0) ? cmd->scale : 1.0;

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
                float w = def[3]/2 * scale;
                float h = def[4]/2 * scale;
                float x = def[1] * scale;
                float y = def[2] * scale;

                b2Vec2 center(x + w, y + h);
                poly.SetAsBox(w, h, center, 0.0);
                fixdef.shape = &poly;
            }
                def += 4;
                break;

            case GK_PATH_CIRCLE:
                circle.m_p = b2Vec2(def[1]*scale, def[2]*scale);
                circle.m_radius = def[3] * scale;
                fixdef.shape = &circle;
                def += 3;
                break;

            case GK_PATH_MOVE_TO:
            case GK_PATH_LINE_TO:
                verts.emplace_back(def[1]*scale, def[2]*scale);
                def += 2;
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

            case GK_PATH_SENSOR:
                fixdef.isSensor = true;
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

            case GK_PATH_FIXTURE_ID: {
                auto data = ensure_fixdata(&fixdef);
                data->id = (int)def[1];
                def++;
            }
                break;

            case GK_PATH_FILL:
                if(!fixdef.shape) {
                    poly.Set(verts.data(), verts.size());
                    fixdef.shape = &poly;
                }

                body->CreateFixture(&fixdef);
                break;

            case GK_PATH_STROKE:
                chain.CreateChain(verts.data(), verts.size());
                fixdef.shape = &chain;
                body->CreateFixture(&fixdef);
                break;

            case GK_PATH_TF_SCALE:
                scale = def[1];
                def++;
                break;

            case GK_PATH_BEGIN:
                new (&fixdef) b2FixtureDef;
                new (&chain) b2ChainShape;
                new (&poly) b2PolygonShape;

                verts.clear();
                break;

            // Don't worry about unusable bits for now on the off chance
            // reusing paths for drawing and physics is useful.
            default: break;
        }
    }
}

void gk_process_b2_fixture_update(gk_context *, gk_cmd_b2_fixture_update* cmd) {
    auto b = ((gk_b2_body_data*)cmd->body->data)->body;
    auto mask = cmd->update;
    auto id = cmd->id;

    // Not the best, but for now
    for(auto f = b->GetFixtureList(); f; f = f->GetNext()) {
        gk_b2_fixture_data *d = (gk_b2_fixture_data*)f->GetUserData();
        if((d && d->id == id) || (!d && id == 0)) {
            if(mask & GK_B2_FIXTURE_UPDATE_DENSITY)    f->SetDensity(cmd->density);
            if(mask & GK_B2_FIXTURE_UPDATE_ELASTICITY) f->SetRestitution(cmd->elasticity);
            if(mask & GK_B2_FIXTURE_UPDATE_FRICTION)   f->SetFriction(cmd->friction);
            if(mask & GK_B2_FIXTURE_UPDATE_SENSOR)     f->SetSensor(cmd->sensor);

            if(mask & GK_B2_FIXTURE_UPDATE_FILTER) {
                b2Filter filt;
                filt.categoryBits = cmd->category;
                filt.maskBits = cmd->mask;
                filt.groupIndex = cmd->group;
                f->SetFilterData(filt);
            }
        }
    }
}

void gk_process_b2_draw_debug(gk_context *gk, gk_cmd_b2_draw_debug *cmd) {
    auto world = cmd->world->data->world;
    auto draw = cmd->world->data->draw;

    float xscale = cmd->scale.x ? cmd->scale.x : 1.0;
    float yscale = cmd->scale.y ? cmd->scale.y : 1.0;

    draw->setSize(cmd->resolution.x, cmd->resolution.y, xscale, yscale);

    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(gk->nvg, cmd->resolution.x, cmd->resolution.y, 1.0);
    nvgTransform(gk->nvg, 1, 0, 0, -1, 0, cmd->resolution.y);

    nvgScale(gk->nvg, xscale, yscale);
    nvgTranslate(gk->nvg, cmd->translate.x, cmd->translate.y);

    world->DrawDebugData();
    nvgEndFrame(gk->nvg);
}

void gk_process_b2_step(gk_context *, gk_cmd_b2_step *cmd) {
    auto world = cmd->world;
    auto data = world->data;
    auto b2world = data->world;
    auto listen = data->listen;
    listen->begin();
    b2world->Step(world->timestep,
                  world->velocity_iterations,
                  world->position_iterations);
    listen->finish(cmd);
}

void gk_process_b2_iter_bodies(gk_context *, gk_cmd_b2_iter_bodies* cmd) {
    auto world = cmd->world->data->world;

    for(auto body = world->GetBodyList(); body; body = body->GetNext()) {
        auto *b = (gk_b2_body*)body->GetUserData();
        auto isAwake = b->is_awake = body->IsAwake();

        if(isAwake) {
            if(b->position)
                *(b2Vec2*)(b->position) = body->GetPosition();
            if(b->angle)
                *(b->angle) = body->GetAngle();
            (b2Vec2&)(b->velocity) = body->GetLinearVelocity();
            b->angular_velocity = body->GetAngularVelocity();
        }
    }
}

void gk_process_b2_force(gk_context *, gk_cmd_b2_force* cmd) {
    cmd->body->data->body->ApplyForce((b2Vec2&)cmd->force,
                                      (b2Vec2&)cmd->point,
                                      cmd->wake);
}

void gk_process_b2_torque(gk_context *, gk_cmd_b2_torque* cmd) {
    cmd->body->data->body->ApplyTorque(cmd->torque, cmd->wake);
}

void gk_process_b2_linear_impulse(gk_context *, gk_cmd_b2_linear_impulse* cmd) {
    cmd->body->data->body->ApplyLinearImpulse((b2Vec2&)cmd->impulse,
                                              (b2Vec2&)cmd->point,
                                              cmd->wake);
}

void gk_process_b2_angular_impulse(gk_context *, gk_cmd_b2_angular_impulse* cmd) {
    cmd->body->data->body->ApplyAngularImpulse(cmd->impulse, cmd->wake);
}

void gk_process_b2_set_velocity(gk_context *, gk_cmd_b2_set_velocity* cmd) {
    cmd->body->data->body->SetLinearVelocity((b2Vec2&)cmd->linear);
    cmd->body->data->body->SetAngularVelocity(cmd->angular);
}

void gk_process_box2d(gk_context *gk, gk_bundle *bundle, gk_list *list) {
    for(size_t j = 0; j < list->ncmds; ++j) {
        auto cmd = list->cmds[j];
        auto type = GK_CMD_TYPE(cmd);

        switch(type) {
            case GK_CMD_B2_BODY_CREATE:
                gk_process_b2_body_create(gk, (gk_cmd_b2_body_create*)cmd);
                break;

            case GK_CMD_B2_BODY_UPDATE:
                gk_process_b2_body_update(gk, (gk_cmd_b2_body_update*)cmd);
                break;

            case GK_CMD_B2_BODY_DESTROY:
                gk_process_b2_body_destroy(gk, (gk_cmd_b2_body_destroy*)cmd);
                break;

            case GK_CMD_B2_FIXTURE_CREATE:
                gk_process_b2_fixture_create(gk, (gk_cmd_b2_fixture_create*)cmd);
                break;

            case GK_CMD_B2_FIXTURE_UPDATE:
                gk_process_b2_fixture_update(gk, (gk_cmd_b2_fixture_update*)cmd);
                break;

            case GK_CMD_B2_DRAW_DEBUG:
                gk_process_b2_draw_debug(gk, (gk_cmd_b2_draw_debug*)cmd);
                break;

            case GK_CMD_B2_STEP:
                gk_process_b2_step(gk, (gk_cmd_b2_step*)cmd);
                break;

            case GK_CMD_B2_ITER_BODIES:
                gk_process_b2_iter_bodies(gk, (gk_cmd_b2_iter_bodies*)cmd);
                break;

            case GK_CMD_B2_FORCE:
                gk_process_b2_force(gk, (gk_cmd_b2_force*)cmd);
                break;

            case GK_CMD_B2_TORQUE:
                gk_process_b2_torque(gk, (gk_cmd_b2_torque*)cmd);
                break;

            case GK_CMD_B2_LINEAR_IMPULSE:
                gk_process_b2_linear_impulse(gk, (gk_cmd_b2_linear_impulse*)cmd);
                break;

            case GK_CMD_B2_ANGULAR_IMPULSE:
                gk_process_b2_angular_impulse(gk, (gk_cmd_b2_angular_impulse*)cmd);
                break;

            case GK_CMD_B2_SET_VELOCITY:
                gk_process_b2_set_velocity(gk, (gk_cmd_b2_set_velocity*)cmd);
                break;

            default:
                gk_process_cmd_general("GK_LIST_BOX2D", gk, bundle, cmd);
                break;
        }
    }
}
