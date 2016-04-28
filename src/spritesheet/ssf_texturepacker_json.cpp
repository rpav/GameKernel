#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <algorithm>

#include "nanovg.h"

#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/spritesheet.hpp"
#include "gk/log.hpp"
#include "json.hpp"

using json = nlohmann::json;

static inline float translate_anchor(float a, float w, float nx, float nw) {
    float p = a * w;
    return (p - nx) / nw;
}

static void parse_texturepacker_frame(const json &meta,
                                      const json &frame,
                                      gk_sprite *sprite,
                                      unsigned int flags) {
    auto &rect    = frame["frame"];
    auto &srcRect = frame["spriteSourceSize"];
    auto &anchor  = frame["pivot"];
    auto &vsize   = frame["sourceSize"];
    bool rotated  = frame["rotated"];

    float mw = meta["size"]["w"];
    float mh = meta["size"]["h"];

    // Based on tests, x,y are always upper-left; width/height are
    // given as per the source material, but must be reversed for
    // packed sheets based on "rotated".

    float ax = anchor["x"], ay = anchor["y"];
    float vw = vsize["w"], vh = vsize["h"];
    float x = rect["x"], y = rect["y"], w = rect["w"], h = rect["h"];

    sprite->size.set(w, h);
    sprite->vsize.set(vw, vh);
    sprite->anchor.set(ax, ay, 0);

    float nax = translate_anchor(ax, vw, srcRect["x"], w);
    float nay = translate_anchor(ay, vh, srcRect["y"], h);

    auto *qv = sprite->attr;

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

    qv[0].vertex.set(fx0, fy0, 0, 1);
    qv[1].vertex.set(fx1, fy0, 0, 1);
    qv[2].vertex.set(fx0, fy1, 0, 1);
    qv[3].vertex.set(fx1, fy1, 0, 1);

    if(rotated) std::swap(w, h);

    float u0, v0, u1, v1;
    u0 = x/mw;
    v0 = y/mh;
    u1 = (x+w)/mw;
    v1 = (y+h)/mh;

    if(rotated) {
        qv[0].uv.set(u1, v0);
        qv[1].uv.set(u1, v1);
        qv[2].uv.set(u0, v0);
        qv[3].uv.set(u0, v1);        
    } else {
        qv[0].uv.set(u0, v0);
        qv[1].uv.set(u1, v0);
        qv[2].uv.set(u0, v1);
        qv[3].uv.set(u1, v1);
    }
}

void gk_load_ssf_texturepacker_json(gk_context *gk, gk_cmd_spritesheet_create *cmd,
                                    gk_spritesheet *sheet) {
    auto path = std::string(cmd->path ? cmd->path : "");

    std::ifstream f(path + "/" + cmd->filename);
    json j;
    j << f;
    auto &frames = j["frames"];
    auto &meta = j["meta"];
    auto imagefile = path + "/" + meta["image"].get<std::string>();

    sheet->tex = nvgCreateImage(gk->nvg, imagefile.c_str(), 0);
    if(cmd->flags & GK_SCF_MIN_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if(cmd->flags & GK_SCF_MAG_LINEAR)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    sheet->nsprites = j["frames"].size();
    sheet->sprites  = (gk_sprite*)malloc(sizeof(gk_sprite) * sheet->nsprites);
    sheet->names    = (char**)malloc(sizeof(char*) * sheet->nsprites);

    int i = 0;
    for(auto it = frames.begin(); it != frames.end(); ++it, ++i) {
        parse_texturepacker_frame(meta, it.value(), &sheet->sprites[i], cmd->flags);
        sheet->names[i] = strdup(it.key().c_str());
    }
}
