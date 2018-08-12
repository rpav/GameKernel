#include <algorithm>
#include <fstream>

#include <rpav/log.hpp>
#include <stdlib.h>
#include <string.h>

#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/spritesheet.hpp"
#include "json.hpp"
#include "nanovg.h"

using namespace rpav;
using json = nlohmann::json;

static inline float translate_anchor(float a, float w, float nx, float nw)
{
    float p = a * w;
    return (p - nx) / nw;
}

static void parse_texturepacker_frame(const json&  meta,
                                      const json&  frame,
                                      gk_sprite*   sprite,
                                      unsigned int flags)
{
    auto& rect    = frame["frame"];
    auto& srcRect = frame["spriteSourceSize"];
    auto& anchor  = frame["pivot"];
    auto& vsize   = frame["sourceSize"];
    bool  rotated = frame["rotated"];

    float mw = meta["size"]["w"];
    float mh = meta["size"]["h"];

    // Based on tests, x,y are always upper-left; width/height are
    // given as per the source material, but must be reversed for
    // packed sheets based on "rotated".

    float ax = anchor["x"], ay = anchor["y"];
    float vw = vsize["w"], vh = vsize["h"];
    float x = rect["x"], y = rect["y"], w = rect["w"], h = rect["h"];

    sprite->size   = {w, h};
    sprite->vsize  = {vw, vh};
    sprite->anchor = {ax, ay, 0};

    float nax = translate_anchor(ax, vw, srcRect["x"], w);
    float nay = translate_anchor(ay, vh, srcRect["y"], h);

    auto* qv = sprite->attr;

    float fx0, fy0, fx1, fy1;
    fx1 = (1 - nax) * w;
    fx0 = fx1 - w;

    fy1 = (1 - nay) * h;
    fy0 = fy1 - h;

    if(flags & GK_SCF_FLIP_X) {
        fx0 = -fx0;
        fx1 = -fx1;
    }

    if(flags & GK_SCF_FLIP_Y) {
        fy0 = -fy0;
        fy1 = -fy1;
    }

    qv[0].vertex = {fx0, fy0, 0, 1};
    qv[1].vertex = {fx1, fy0, 0, 1};
    qv[2].vertex = {fx0, fy1, 0, 1};
    qv[3].vertex = {fx1, fy1, 0, 1};

    if(rotated) std::swap(w, h);

    float u0, v0, u1, v1;
    u0 = x / mw;
    v0 = y / mh;
    u1 = (x + w) / mw;
    v1 = (y + h) / mh;

    if(rotated) {
        qv[0].uv = {u1, v0};
        qv[1].uv = {u1, v1};
        qv[2].uv = {u0, v0};
        qv[3].uv = {u0, v1};
    } else {
        qv[0].uv = {u0, v0};
        qv[1].uv = {u1, v0};
        qv[2].uv = {u0, v1};
        qv[3].uv = {u1, v1};
    }
}

void gk_load_ssf_texturepacker_json(gk_context*                gk,
                                    gk_cmd_spritesheet_create* cmd,
                                    gk_spritesheet*            sheet)
{
    std::ifstream f(cmd->path);
    json          j;
    f >> j;
    auto& frames = j["frames"];
    auto& meta   = j["meta"];

    std::string path(cmd->path);
    auto        dirname   = path.substr(0, path.find_last_of("\\/") + 1);
    auto        imagefile = dirname + meta["image"].get<std::string>();
    int         i         = 0;

    sheet->tex = nvgCreateImage(gk->nvg, imagefile.c_str(), 0);
    glBindTexture(GL_TEXTURE_2D, sheet->tex);
    if(cmd->flags & GK_SCF_MIN_NEAREST)
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    else
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

    if(cmd->flags & GK_SCF_MAG_LINEAR)
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    else
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    sheet->nsprites = j["frames"].size();
    sheet->sprites  = (gk_sprite*)malloc(sizeof(gk_sprite) * sheet->nsprites);
    sheet->names    = (char**)malloc(sizeof(char*) * sheet->nsprites);

    i = 0;
    for(auto it = frames.begin(); it != frames.end(); ++it, ++i) {
        parse_texturepacker_frame(meta, it.value(), &sheet->sprites[i], cmd->flags);
        sheet->names[i] = strdup(it.key().c_str());
    }

    return;

gl_error:
    if(sheet->tex) glDeleteTextures(1, &sheet->tex);

    return;
}
