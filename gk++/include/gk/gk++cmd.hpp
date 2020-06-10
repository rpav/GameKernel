#pragma once

#include <map>
#include <string>
#include <vector>

#include <rpav/ptr.hpp>
#include <rpav/util.hpp>

#include "gk/gk++list.hpp"
#include "gk/gk++util.hpp"
#include "gk/gk.h"

namespace gk {
template<typename T, gk_cmd_type ID>
class CmdTmpl : public CmdBase {
public:
    CmdTmpl(int key = 0)
    {
        rpav::memzero(cmd);
        auto* c = (gk_cmd*)&cmd;
        c->type = ID;
        c->key  = key;
    }

    gk_cmd* cmdPtr() override { return (gk_cmd*)&cmd; }

    void key(int key) { cmdPtr()->key = key; }

    T cmd;
};

/* gk::CmdPass */
class CmdPass : public CmdTmpl<gk_pass, GK_CMD_PASS> {
public:
    CmdPass(unsigned int index, gk_pass_sorting sort = GK_PASS_SORT_NONE)
    {
        cmd.list_index = index;
        cmd.sort       = sort;
    }

    inline void index(unsigned int index) { cmd.list_index = index; }
    inline void sort(gk_pass_sorting sort) { cmd.sort = sort; }
};

/* gk::Passes */
class Passes : public MultiCmd {
    std::vector<CmdPass> _passes;

public:
    template<typename... Rest>
    inline void add(unsigned int index, Rest... args)
    {
        _passes.emplace_back(index);
        add(args...);
    }

    inline void add() {}

    CmdPass& operator[](size_t i) { return _passes[i]; }

    void addToList(ListBase& list) override
    {
        for(auto& cmd : _passes) {
            list.addCmd(cmd);
        }
        list.updateList();
    }
};

// gk::CmdFunction
class CmdFunction : public CmdTmpl<gk_cmd_function, GK_CMD_FUNCTION> {
public:
    using function_type = decltype(gk_cmd_function::function);
    CmdFunction(function_type f, void* data)
    {
        cmd.function = f;
        cmd.data     = data;
    }
};


// gk::CmdClear
class CmdClear : public CmdTmpl<gk_cmd_clear, GK_CMD_CLEAR> {
public:
    CmdClear(uint32_t flags)
    {
        cmd.flags = flags;
        cmd.depth = 1.0;
    }

    CmdClear(float r, float g, float b, float a = 1.0) : CmdClear(GK_CLEAR_COLOR)
    {
        cmd.color.x = r;
        cmd.color.y = g;
        cmd.color.z = b;
        cmd.color.w = a;
    }
};

template<typename T, gk_cmd_type ID>
class CmdTF : public CmdTmpl<T, ID> {
public:
    CmdTF() : CmdTmpl<T, ID>()
    {
        auto* tf  = (gk_cmd_tf*)&this->cmd;
        tf->prior = nullptr;
        tf->out   = nullptr;
    }

    inline void setOut(gk_mat4& m)
    {
        auto* tf = (gk_cmd_tf*)&this->cmd;
        tf->out  = &m;
    }

    inline void setPrior(const gk_mat4& m)
    {
        auto* tf  = (gk_cmd_tf*)&this->cmd;
        tf->prior = const_cast<gk_mat4*>(&m);
    }
};

// gk::CmdTFOrtho
class CmdTFOrtho : public CmdTF<gk_cmd_tf_ortho, GK_CMD_TF_ORTHO> {
public:
    CmdTFOrtho(float left, float right, float bottom, float top, float near, float far)
    {
        cmd.left   = left;
        cmd.right  = right;
        cmd.bottom = bottom;
        cmd.top    = top;
        cmd.near   = near;
        cmd.far    = far;
    }
};

// gk::CmdTFTRS
class CmdTFTRS : public CmdTF<gk_cmd_tf_trs, GK_CMD_TF_TRS> {
public:
    void translate(const vec3& v) { cmd.translate = v; }

    void rotate(const vec3& axis, float angle)
    {
        if(angle) {
            cmd.flags |= GK_TRS_ROTATE;
            cmd.axis  = axis;
            cmd.angle = angle;
        } else {
            cmd.flags &= ~GK_TRS_ROTATE;
        }
    }

    void scale(const vec3& v)
    {
        if(v.x == 1.0 && v.y == 1.0 && v.z == 1.0) {
            cmd.flags &= ~GK_TRS_SCALE;
        } else {
            cmd.flags |= GK_TRS_SCALE;
            cmd.scale = v;
        }
    }
};

// gk::CmdTFM
class CmdTFM : public CmdTF<gk_cmd_tf_m, GK_CMD_TF_M> {
public:
    CmdTFM(mat4* m = nullptr) { cmd.m = m; }

    void setM(mat4* m) { cmd.m = m; }
};

// gk::CmdQuad
class CmdQuad : public CmdTmpl<gk_cmd_quad, GK_CMD_QUAD> {
public:
    CmdQuad(int tex = 0)
    {
        cmd.tex = tex;
        cmd.tfm = gk::mat4{};
    }

    // For trivial quads
    CmdQuad(
        unsigned int tex,
        float        x0,
        float        y0,
        float        x1,
        float        y1,
        float        u0 = 0.0,
        float        v0 = 0.0,
        float        u1 = 1.0,
        float        v1 = 1.0)
        : CmdQuad(tex)
    {
        setVertex(0, x0, y0);
        setVertex(1, x1, y0);
        setVertex(2, x0, y1);
        setVertex(3, x1, y1);

        setUV(0, u0, v0);
        setUV(1, u1, v0);
        setUV(2, u0, v1);
        setUV(3, u1, v1);
    }

    inline void setVertex(int n, gk_vec4& v) { cmd.attr[n].vertex = v; }

    inline void setVertex(int n, float x, float y, float z = 0.0, float w = 1.0)
    {
        cmd.attr[n].vertex.x = x;
        cmd.attr[n].vertex.y = y;
        cmd.attr[n].vertex.z = z;
        cmd.attr[n].vertex.w = w;
    }

    inline void setUV(int n, gk_vec2& v) { cmd.attr[n].uv = v; }

    inline void setUV(int n, float u, float v)
    {
        cmd.attr[n].uv.x = u;
        cmd.attr[n].uv.y = v;
    }
};

class CmdQuadSprite : public CmdTmpl<gk_cmd_quadsprite, GK_CMD_QUADSPRITE> {
public:
    CmdQuadSprite() = default;

    CmdQuadSprite(gk::SpriteSheet& sheet, unsigned int index)
    {
        cmd.tfm   = gk::mat4{};
        cmd.sheet = sheet.sheet;
        cmd.index = index;
    }

    void setSheet(gk::SpriteSheet& sheet) {}

    mat4* tfm() { return (mat4*)&cmd.tfm; }
};

class CmdSpriteLayer : public CmdTmpl<gk_cmd_spritelayer, GK_CMD_SPRITELAYER> {
public:
    std::vector<uint32_t> sprites;

    CmdSpriteLayer(gk_spritelayer_config* config, gk_spritelayer_render* render)
        : sprites((size_t)config->layer_size.x * (size_t)config->layer_size.y, 0)
    {
        cmd.config  = config;
        cmd.render  = render;
        cmd.sprites = nullptr;
    }

    CmdSpriteLayer()
    {
        cmd.config  = nullptr;
        cmd.render  = nullptr;
        cmd.sprites = nullptr;
    }

    void reserve(size_t size) { sprites.reserve(size); }
    void resize(size_t size) { sprites.resize(size); }

    void copy(std::vector<uint32_t> data)
    {
        sprites     = std::move(data);
        cmd.sprites = sprites.data();
    }

    void fill(uint32_t v) {
        std::fill(sprites.begin(), sprites.end(), v);
        cmd.sprites = sprites.data();
    }
};

class CmdChunkLayer : public CmdTmpl<gk_cmd_chunklayer, GK_CMD_CHUNKLAYER> {
    std::vector<gk_spritechunk> _chunks;

public:
    CmdChunkLayer() : CmdTmpl() { cmd.tfm = gk::mat4{}; }

    CmdChunkLayer(const CmdChunkLayer&) = default;
    CmdChunkLayer(CmdChunkLayer&&)      = default;

    // Note this is *not* set up until you call resize(); if you initialize
    // with resize().  You should also likely .zero().  You may thus initialize
    // the config/render pointers without initializing config/render data or
    // preallocating anything.
    CmdChunkLayer(gk_chunklayer_config* config, gk_spritelayer_render* render)
        : CmdChunkLayer()
    {
        cmd.config = config;
        cmd.render = render;
        cmd.chunks = nullptr;
    }

    CmdChunkLayer& resize(size_t n)
    {
        _chunks.resize(n);
        cmd.nchunks = n;
        cmd.chunks  = _chunks.data();
        return *this;
    }

    CmdChunkLayer& zero()
    {
        for(auto& sc : _chunks) {
            sc.sprites = nullptr;
        }
        return *this;
    }

    gk_spritechunk& operator[](size_t x) { return _chunks[x]; }

    /* This is inefficient, linear in the number of chunks.  This isn't so bad you can't look
       up the occasional tile.  Just don't do it for every tile... if you need that, iterate
       the chunks!
    */
    rpav::ptr<gk_sprite_id> get(gk::ivec2 v);
};

/* gk::CmdPath */

class PathDef : public std::vector<float> {
    inline void _append() {}

    template<typename T, typename... Ts>
    inline void _append(T t, Ts... ts)
    {
        emplace_back(static_cast<float>(t));
        _append(ts...);
    }

public:
    template<typename... Ts>
    PathDef(Ts... ts)
    {
        reserve(sizeof...(Ts));
        _append(ts...);
    }

    template<typename T>
    inline PathDef& append(T f)
    {
        emplace_back(static_cast<float>(f));
        return *this;
    }

    template<typename... Ts>
    inline PathDef& append(Ts... ts)
    {
        reserve(size() + sizeof...(Ts));
        _append(ts...);
        return *this;
    }

    inline PathDef& append(const PathDef& other)
    {
        insert(end(), other.cbegin(), other.cend());
        return *this;
    }

    inline PathDef& begin()
    {
        append(GK_PATH_BEGIN);
        return *this;
    }
    inline PathDef& save()
    {
        append(GK_PATH_SAVE);
        return *this;
    }
    inline PathDef& restore()
    {
        append(GK_PATH_RESTORE);
        return *this;
    }
    inline PathDef& stroke()
    {
        append(GK_PATH_STROKE);
        return *this;
    }
    inline PathDef& fill()
    {
        append(GK_PATH_FILL);
        return *this;
    }

    inline PathDef& rect(float x, float y, float h, float w)
    {
        append({GK_PATH_RECT, x, y, h, w});
        return *this;
    }

    inline PathDef& rect(const vec2& pos, const vec2& size)
    {
        append({GK_PATH_RECT, pos.x, pos.y, size.x, size.y});
        return *this;
    }

    inline PathDef& rect(const gk::rect r)
    {
        append({GK_PATH_RECT, r.pos.x, r.pos.y, r.size.x, r.size.y});
        return *this;
    }

    inline PathDef& circle(float x, float y, float r)
    {
        append({GK_PATH_CIRCLE, x, y, r});
        return *this;
    }

    inline PathDef& circle(const vec2& pos, float r)
    {
        append({GK_PATH_CIRCLE, pos.x, pos.y, r});
        return *this;
    }

    inline PathDef& lineTo(float x, float y)
    {
        append({GK_PATH_LINE_TO, x, y});
        return *this;
    }

    inline PathDef& lineTo(const vec2& v)
    {
        append({GK_PATH_LINE_TO, v.x, v.y});
        return *this;
    }

    inline PathDef& moveTo(float x, float y)
    {
        append({GK_PATH_MOVE_TO, x, y});
        return *this;
    }

    inline PathDef& moveTo(const vec2& v)
    {
        append({GK_PATH_MOVE_TO, v.x, v.y});
        return *this;
    }

    inline PathDef& translate(float x, float y)
    {
        append({GK_PATH_TF_TRANSLATE, x, y});
        return *this;
    }

    inline PathDef& translate(const vec2& v)
    {
        append({GK_PATH_TF_TRANSLATE, v.x, v.y});
        return *this;
    }

    inline PathDef& rotate(float r)
    {
        append({GK_PATH_TF_ROTATE, r});
        return *this;
    }

    inline PathDef& scale(float x, float y)
    {
        append({GK_PATH_TF_SCALE, x, y});
        return *this;
    }

    inline PathDef& identity()
    {
        append({GK_PATH_TF_IDENTITY});
        return *this;
    }

    inline PathDef& fillColorF(float r, float g, float b, float a = 1.0)
    {
        append({GK_PATH_FILL_COLOR_RGBAF, r, g, b, a});
        return *this;
    }

    inline PathDef& fillColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
    {
        append({GK_PATH_FILL_COLOR_RGBA, (float)r, (float)g, (float)b, (float)a});
        return *this;
    }

    inline PathDef& strokeColorF(float r, float g, float b, float a = 1.0)
    {
        append({GK_PATH_STROKE_COLOR_RGBAF, r, g, b, a});
        return *this;
    }

    inline PathDef& strokeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
    {
        append({GK_PATH_STROKE_COLOR_RGBA, (float)r, (float)g, (float)b, (float)a});
        return *this;
    }

    inline PathDef& strokeWidth(float f)
    {
        append({GK_PATH_STROKE_WIDTH, f});
        return *this;
    }

    inline PathDef& close()
    {
        append({GK_PATH_CLOSE});
        return *this;
    }

    /* Physics */
    inline PathDef& fixtureId(int id)
    {
        append({GK_PATH_FIXTURE_ID, (float)id});
        return *this;
    }

    inline PathDef& category(unsigned int c)
    {
        append({GK_PATH_CATEGORY, (float)c});
        return *this;
    }

    inline PathDef& categoryMask(unsigned short m)
    {
        append({GK_PATH_CATEGORY_MASK, (float)m});
        return *this;
    }

    inline PathDef& group(int16_t group)
    {
        append({GK_PATH_GROUP, (float)group});
        return *this;
    }

    inline PathDef& density(float f)
    {
        append({GK_PATH_DENSITY, f});
        return *this;
    }

    inline PathDef& friction(float f)
    {
        append({GK_PATH_FRICTION, f});
        return *this;
    }

    inline PathDef& sensor()
    {
        append({GK_PATH_SENSOR});
        return *this;
    }

    /* FIXME: add more */
};

class CmdPath : public CmdTmpl<gk_cmd_path, GK_CMD_PATH> {
public:
    void setPath(float* pathdef, size_t count)
    {
        cmd.pathdef = pathdef;
        cmd.pathlen = count;
    }

    void setPath(PathDef& pathdef)
    {
        cmd.pathdef = pathdef.data();
        cmd.pathlen = pathdef.size();
    }
};

// gk::CmdFontCreate
class CmdFontCreate : public CmdTmpl<gk_cmd_font_create, GK_CMD_FONT_CREATE> {
public:
    CmdFontCreate(const char* name, const char* filename)
    {
        cmd.name     = name;
        cmd.filename = filename;
    }
};

// gk::CmdFontStyle
class CmdFontStyle : public CmdTmpl<gk_cmd_font_style, GK_CMD_FONT_STYLE> {
public:
    CmdFontStyle(float size = 0.0)
    {
        cmd.size        = size;
        cmd.spacing     = 1.0;
        cmd.line_height = 1.0;
        cmd.align       = GK_ALIGN_LEFT | GK_ALIGN_BASELINE;
    }
};

// gk::CmdText
class CmdText : public CmdTmpl<gk_cmd_text, GK_CMD_TEXT> {
public:
    CmdText(const char* string = nullptr, float x = 0.0, float y = 0.0)
    {
        cmd.str   = string;
        cmd.pos.x = x;
        cmd.pos.y = y;
    }

    CmdText(const char* string, gk_vec2& pos)
    {
        cmd.str = string;
        cmd.pos = pos;
    }
};

// gk::CmdImageCreate
class CmdImageCreate : public CmdTmpl<gk_cmd_image_create, GK_CMD_IMAGE_CREATE> {
    std::string _filename;

public:
    CmdImageCreate(
        const std::string filename,
        uint32_t          tex_flags = 0,
        gk_tex_filter     min       = GK_TEX_FILTER_LINEAR,
        gk_tex_filter     mag       = GK_TEX_FILTER_LINEAR)
        : _filename(filename)
    {
        cmd.filename   = _filename.c_str();
        cmd.flags      = tex_flags;
        cmd.min_filter = min;
        cmd.mag_filter = mag;
    }

    int id() const { return cmd.id; }
};

// gk::CmdImageDestroy
typedef std::vector<int> ImageIDVector;
class CmdImageDestroy : public CmdTmpl<gk_cmd_image_destroy, GK_CMD_IMAGE_DESTROY> {
    ImageIDVector ids;

public:
    CmdImageDestroy(CmdImageCreate& create) { add(create); }

    void add(int id)
    {
        ids.push_back(id);
        cmd.ids  = ids.data();
        cmd.nids = ids.size();
    }

    void add(CmdImageCreate& cmd) { add(cmd.id()); }
};

// gk::CmdNvgRawFunction
class CmdNvgRawFunction : public CmdTmpl<gk_cmd_nvg_function, GK_CMD_NVG_FUNCTION> {
public:
    using function_type = decltype(gk_cmd_nvg_function::function);
    CmdNvgRawFunction(function_type f, void* data)
    {
        cmd.function = f;
        cmd.data     = data;
    }
};

// gk::CmdNvgFunction
// Slower but more convenient for one-offs
class CmdNvgFunction : public CmdTmpl<gk_cmd_nvg_function, GK_CMD_NVG_FUNCTION> {
public:
    using function_type = std::function<void(NVGcontext*)>;
    function_type _function;

private:
    static void _callback(NVGcontext* nvg, void* data)
    {
        auto* cmd = reinterpret_cast<CmdNvgFunction*>(data);
        cmd->_function(nvg);
    }

public:
    CmdNvgFunction(function_type f)
    {
        _function    = std::move(f);
        cmd.function = _callback;
        cmd.data     = this;
    }
};

// gk::B2World
struct B2World : public gk_b2_world {
    B2World(
        float timestep           = 1.0f / 60.0f,
        int   velocityIterations = 8,
        int   positionIterations = 3)
        : gk_b2_world()
    {
        this->timestep            = timestep;
        this->velocity_iterations = velocityIterations;
        this->position_iterations = positionIterations;
        this->data                = nullptr;
    }
};

/* gk::CmdB2WorldCreate */
class CmdB2WorldCreate : public CmdTmpl<gk_cmd_b2_world_create, GK_CMD_B2_WORLD_CREATE> {
public:
    CmdB2WorldCreate(gk_b2_world& world, const vec2& gravity, bool doSleep)
    {
        cmd.world    = &world;
        cmd.gravity  = gravity;
        cmd.do_sleep = (char)doSleep;
    }
};

/* gk::CmdB2WorldDestroy */
class CmdB2WorldDestroy : public CmdTmpl<gk_cmd_b2_world_destroy, GK_CMD_B2_WORLD_DESTROY> {
public:
    CmdB2WorldDestroy(gk_b2_world& world) { cmd.world = &world; }
};

// gk::B2Body
struct B2Body : public gk_b2_body {
    B2Body()
    {
        angle            = 0;
        angular_velocity = 0;
        is_awake         = false;
        user_data        = nullptr;
        data             = nullptr;
    }
};

struct B2BodyDef : public gk_b2_bodydef {
    B2BodyDef(B2Body& b, gk_b2_body_type type_ = GK_B2_BODY_TYPE_STATIC)
    {
        type = type_;

        linear_damping   = 0;
        angle            = 0;
        angular_velocity = 0;
        angular_damping  = 0;

        gravity_scale = 1.0;

        active         = true;
        awake          = true;
        bullet         = false;
        fixed_rotation = false;
        allow_sleep    = true;

        body = &b;
    }
};

// gk::CmdB2BodyCreate
using BodydefVector = std::vector<gk_b2_bodydef*>;
class CmdB2BodyCreate : public CmdTmpl<gk_cmd_b2_body_create, GK_CMD_B2_BODY_CREATE> {
    BodydefVector defs;

public:
    CmdB2BodyCreate(gk_b2_world& world) { cmd.world = &world; }

    template<typename... Rest>
    inline void add(B2BodyDef& bd, Rest&... args)
    {
        defs.push_back(&bd);
        add(args...);
    }

    inline void add()
    {
        cmd.ndefs = defs.size();
        cmd.defs  = defs.data();
    }
};

class CmdB2BodyDestroy : public CmdTmpl<gk_cmd_b2_body_destroy, GK_CMD_B2_BODY_DESTROY> {
    std::vector<gk_b2_body*> bodies;

public:
    CmdB2BodyDestroy(gk_b2_world& world) { cmd.world = &world; }

    template<typename... Rest>
    inline void add(gk_b2_body& bd, Rest&&... args)
    {
        bodies.push_back(&bd);
        add(args...);
    }

    inline void add()
    {
        cmd.nbodies = bodies.size();
        cmd.bodies  = bodies.data();
    }
};

// gk::CmdB2BodyUpdate
class CmdB2BodyUpdate : public CmdTmpl<gk_cmd_b2_body_update, GK_CMD_B2_BODY_UPDATE> {
public:
    CmdB2BodyUpdate(gk_b2_body& body) { cmd.body = &body; }
};

// gk::CmdB2FixtureCreate
class CmdB2FixtureCreate
    : public CmdTmpl<gk_cmd_b2_fixture_create, GK_CMD_B2_FIXTURE_CREATE> {
public:
    CmdB2FixtureCreate(gk_b2_body& body) { cmd.body = &body; }

    void setPath(float* pathdef, size_t count)
    {
        cmd.pathdef = pathdef;
        cmd.pathlen = count;
    }

    void setPath(PathDef& pathdef)
    {
        cmd.pathdef = pathdef.data();
        cmd.pathlen = pathdef.size();
    }
};

// gk::CmdB2FixtureUpdate
class CmdB2FixtureUpdate
    : public CmdTmpl<gk_cmd_b2_fixture_update, GK_CMD_B2_FIXTURE_UPDATE> {
public:
    CmdB2FixtureUpdate() = default;

    CmdB2FixtureUpdate(gk_b2_body& body, int id)
    {
        setBody(body);
        cmd.id = id;
    }

    CmdB2FixtureUpdate(gk_b2_body& body) { setBody(body); }

    CmdB2FixtureUpdate(int id) { cmd.id = id; }

    inline void setBody(gk_b2_body& body) { cmd.body = &body; }
};

// gk::CmdB2Step
class CmdB2Step : public CmdTmpl<gk_cmd_b2_step, GK_CMD_B2_STEP> {
public:
    CmdB2Step(gk_b2_world& world) { cmd.world = &world; }
};

class CmdB2IterBodies : public CmdTmpl<gk_cmd_b2_iter_bodies, GK_CMD_B2_ITER_BODIES> {
public:
    CmdB2IterBodies(gk_b2_world& world) { cmd.world = &world; }
};

class CmdB2Force : public CmdTmpl<gk_cmd_b2_force, GK_CMD_B2_FORCE> {
public:
    CmdB2Force(vec2 force, vec2 point, uint8_t flags = 0)
    {
        cmd.force = (gk_vec2)force;
        cmd.point = (gk_vec2)point;
        cmd.flags = flags;
    }

    CmdB2Force(gk_b2_body& body, vec2 force, vec2 point, uint8_t flags = 0)
        : CmdB2Force(force, point, flags)
    {
        setBody(body);
    }

    inline void setBody(gk_b2_body& body) { cmd.body = &body; }
};

class CmdB2Torque : public CmdTmpl<gk_cmd_b2_torque, GK_CMD_B2_TORQUE> {
public:
    CmdB2Torque(float torque, uint8_t flags = 0)
    {
        cmd.torque = torque;
        cmd.flags  = flags;
    }

    CmdB2Torque(gk_b2_body& body, float torque, uint8_t flags = 0)
        : CmdB2Torque(torque, flags)
    {
        setBody(body);
    }

    inline void setBody(gk_b2_body& body) { cmd.body = &body; }
};

class CmdB2LinearImpulse
    : public CmdTmpl<gk_cmd_b2_linear_impulse, GK_CMD_B2_LINEAR_IMPULSE> {
public:
    CmdB2LinearImpulse(vec2 impulse, vec2 point, bool wake = true)
    {
        cmd.impulse = (gk_vec2)impulse;
        cmd.point   = (gk_vec2)point;
        cmd.wake    = wake;
    }

    CmdB2LinearImpulse(gk_b2_body& body, vec2 impulse, vec2 point, bool wake = true)
        : CmdB2LinearImpulse(impulse, point, wake)
    {
        setBody(body);
    }

    inline void setBody(gk_b2_body& body) { cmd.body = &body; }
};

class CmdB2AngularImpulse
    : public CmdTmpl<gk_cmd_b2_angular_impulse, GK_CMD_B2_ANGULAR_IMPULSE> {
public:
    CmdB2AngularImpulse(float impulse, bool wake = true)
    {
        cmd.impulse = impulse;
        cmd.wake    = wake;
    }

    CmdB2AngularImpulse(gk_b2_body& body, float impulse, bool wake = true)
        : CmdB2AngularImpulse(impulse, wake)
    {
        setBody(body);
    }

    inline void setBody(gk_b2_body& body) { cmd.body = &body; }
};

class CmdB2SetVelocity : public CmdTmpl<gk_cmd_b2_set_velocity, GK_CMD_B2_SET_VELOCITY> {
public:
    CmdB2SetVelocity(vec2 linear, float angular)
    {
        cmd.linear  = linear;
        cmd.angular = angular;
    }

    CmdB2SetVelocity(gk_b2_body& body, vec2 linear, float angular)
        : CmdB2SetVelocity(linear, angular)
    {
        setBody(body);
    }
    inline void setBody(gk_b2_body& body) { cmd.body = &body; }
};

// gk::CmdB2DrawDebug
class CmdB2DrawDebug : public CmdTmpl<gk_cmd_b2_draw_debug, GK_CMD_B2_DRAW_DEBUG> {
public:
    CmdB2DrawDebug(gk_b2_world& world, float width, float height)
    {
        cmd.world        = &world;
        cmd.resolution.x = width;
        cmd.resolution.y = height;
    }
};

// gk::CmdRtCreate
class CmdRtCreate : public CmdTmpl<gk_cmd_rt_create, GK_CMD_RT_CREATE> {
public:
    CmdRtCreate(ivec2 dims, uint32_t flags = GK_RT_ALPHA | GK_RT_STENCIL)
        : CmdRtCreate(dims.x, dims.y, flags)
    {}

    CmdRtCreate(uint32_t width, uint32_t height, uint32_t flags = GK_RT_ALPHA | GK_RT_STENCIL)
    {
        cmd.width  = width;
        cmd.height = height;

        cmd.rt_flags = flags;

        cmd.tex_min_filter = GK_TEX_FILTER_LINEAR;
        cmd.tex_mag_filter = GK_TEX_FILTER_LINEAR;
    }

    CmdRtCreate()                   = default;
    CmdRtCreate(const CmdRtCreate&) = default;
    CmdRtCreate(CmdRtCreate&&)      = default;

    CmdRtCreate& operator=(const CmdRtCreate&) = default;
};

// gk::CmdRtDestroy
class CmdRtDestroy : public CmdTmpl<gk_cmd_rt_destroy, GK_CMD_RT_DESTROY> {
public:
    CmdRtDestroy() = default;

    CmdRtDestroy(CmdRtCreate& create)
    {
        cmd.framebuffer = create.cmd.framebuffer;
        cmd.dsbuffer    = create.cmd.dsbuffer;
        cmd.tex         = create.cmd.tex;
    }
};

// gk::CmdRtBind
class CmdRtBind : public CmdTmpl<gk_cmd_rt_bind, GK_CMD_RT_BIND> {
public:
    CmdRtBind() = default;
    CmdRtBind(uint32_t framebuffer) { cmd.framebuffer = framebuffer; }
    CmdRtBind(CmdRtCreate& create) { cmd.framebuffer = create.cmd.framebuffer; }
};

// gk::CmdRtUnbind
class CmdRtUnbind : public CmdTmpl<gk_cmd_rt_unbind, GK_CMD_RT_UNBIND> {
public:
};

// gk::CmdProgramCreate
typedef std::vector<gk_program_source*> ProgramSourceVector;
class CmdProgramCreate : public CmdTmpl<gk_cmd_program_create, GK_CMD_PROGRAM_CREATE> {
    ProgramSourceVector _sources;

public:
    CmdProgramCreate() = default;
    CmdProgramCreate(ProgramSource& src) { add(src); }

    template<typename... Rest>
    inline void add(ProgramSource& bd, Rest&... args)
    {
        _sources.push_back(&bd.source);
        add(args...);
    }

    inline void add()
    {
        cmd.nsources = _sources.size();
        cmd.source   = _sources.data();
    }
};

// gk::CmdProgramDestroy
typedef std::vector<gk_program> ProgramVector;
class CmdProgramDestroy : public CmdTmpl<gk_cmd_program_destroy, GK_CMD_PROGRAM_DESTROY> {
    ProgramVector _progs;

public:
    CmdProgramDestroy() = default;

    template<typename... Rest>
    inline void add(gk_program& p, Rest&... args)
    {
        _progs.push_back(p);
        add(args...);
    }

    template<typename... Rest>
    inline void add(CmdProgramCreate& cmd, Rest&... args)
    {
        auto& c = cmd.cmd;
        for(size_t i = 0; i < c.nsources; ++i) {
            _progs.push_back(c.source[i]->program);
        }
        add(args...);
    }

    inline void add()
    {
        cmd.nprograms = _progs.size();
        cmd.program   = _progs.data();
    }
};

// gk::CmdUniformQuery
using NameVector    = std::vector<const char*>;
using UniformVector = std::vector<gk_uniform>;

struct strcmp_f {
    bool operator()(const char* a, const char* b) const { return strcmp(a, b) < 0; }
};
using UniformNameLocationMap = std::map<const char*, gk_uniform, strcmp_f>;

class CmdUniformQuery : public CmdTmpl<gk_cmd_uniform_query, GK_CMD_UNIFORM_QUERY> {
    NameVector             _names;
    UniformVector          _uniforms;
    UniformNameLocationMap _locations;

public:
    CmdUniformQuery(gk_program program) { cmd.program = program; }
    CmdUniformQuery(ProgramSource& source) : CmdUniformQuery(source.source.program) {}

    template<typename... Rest>
    inline void add(const char* name, Rest... args)
    {
        _names.push_back(name);
        add(args...);
    }

    inline void add()
    {
        cmd.nuniforms = _names.size();
        cmd.names     = _names.data();

        _uniforms.resize(_names.size());
        cmd.uniforms = _uniforms.data();
    }

    void index()
    {
        _locations.clear();
        for(size_t i = 0; i < cmd.nuniforms; ++i) {
            _locations[cmd.names[i]] = cmd.uniforms[i];
        }
    }

    gk_uniform find(const char* name)
    {
        auto i = _locations.find(name);

        if(i != _locations.end()) return i->second;

        return -1;
    }
};

class CmdDefaultPDS : public CmdTmpl<gk_cmd_default_pds, GK_CMD_DEFAULT_PDS> {};

// gk::CmdSpriteSheetCreate
class CmdSpriteSheetCreate
    : public CmdTmpl<gk_cmd_spritesheet_create, GK_CMD_SPRITESHEET_CREATE> {
    std::string _path;

public:
    CmdSpriteSheetCreate(
        gk_spritesheet_format       fmt,
        std::string                 path,
        gk_spritesheet_create_flags fl = (gk_spritesheet_create_flags)0)
        : _path(std::move(path))
    {
        cmd.format = fmt;
        cmd.flags  = fl;
        cmd.path   = _path.c_str();
    }
};

// gk::CmdSpriteSheetDestroy
using SpriteSheetVector = std::vector<gk_spritesheet*>;
class CmdSpriteSheetDestroy
    : public CmdTmpl<gk_cmd_spritesheet_destroy, GK_CMD_SPRITESHEET_DESTROY> {
    SpriteSheetVector sheets;

public:
    CmdSpriteSheetDestroy() = default;

    CmdSpriteSheetDestroy(gk_spritesheet* sheet) { add(sheet); }

    CmdSpriteSheetDestroy(SpriteSheet& sheet) { add(sheet.sheet); }

    void add(gk_spritesheet* sheet)
    {
        if(!sheet) return;

        sheets.push_back(sheet);
        cmd.sheets  = sheets.data();
        cmd.nsheets = sheets.size();
    }

    void reserve(SpriteSheetVector::size_type size) { sheets.reserve(size); }
};
} // namespace gk
