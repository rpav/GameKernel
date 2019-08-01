#include <cstring>

#include <rpav/log.hpp>
#include <rpav/str/gk.hpp>

#include "gk/gk++.hpp"

using namespace rpav;

BEGIN_NS_GK;

Bundle::Bundle(unsigned int start, gk_pass_sorting sort)
{
    memset(&bundle, 0, sizeof(gk_bundle));

    bundle.start.parent.type = GK_CMD_PASS;
    bundle.start.sort        = sort;
    bundle.start.list_index  = start;
}

void Bundle::handleError()
{
    if(!bundle.error.code) return;

    say("gk++: Error processing bundle (", bundle.error.code, "): ", bundle.error.message);
}

void SpriteSheet::rereadSheet()
{
    if(!sheet) return;

    _name_map.clear();

    if(sheet->names) {
        for(size_t i = 0; i < sheet->nsprites; ++i) _name_map.emplace(sheet->names[i], i + 1);
    }
}

SpriteSheet::SpriteSheet(gk_spritesheet* sheet_) : sheet(sheet_)
{
    rereadSheet();
}

SpriteSheet::SpriteSheet(CmdSpriteSheetCreate& cmd) : sheet(cmd.cmd.sheet)
{
    rereadSheet();
}

rpav::ptr<gk_sprite_id> CmdChunkLayer::get(gk::ivec2 v)
{
    v -= ivec2{cmd.origin};

    auto size = cmd.config->chunk_size;

    for(auto& c : _chunks) {
        auto offset0 = c.offset * size;
        auto offset1 = offset0 + size;

        if(offset0 <= v && v < offset1) {
            v -= offset0;
            v.y = size.y - v.y - 1;
            return &c.sprites[v.y * size.x + v.x];
        }
    }

    return nullptr;
}

END_NS_GK;
