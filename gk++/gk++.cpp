#include <string.h>

#include "gk/gk++.hpp"
#include "gk/log.hpp"

BEGIN_NS_GK;

Bundle::Bundle(unsigned int start, gk_pass_sorting sort) {
    memset(&bundle, 0, sizeof(gk_bundle));

    bundle.start.parent.type = GK_CMD_PASS;
    bundle.start.sort = sort;
    bundle.start.list_index = start;
}

void Bundle::handleError() {
    if(!bundle.error.code) return;

    LOG("gk++: Error processing bundle (",
        bundle.error.code,
        "): ",
        bundle.error.message);
}

void SpriteSheet::rereadSheet() {
    _name_map.clear();

    if(_sheet->names) {
        for(size_t i = 0; i < _sheet->nsprites; ++i)
            _name_map.emplace(_sheet->names[i], i);
    }
}

SpriteSheet::SpriteSheet(gk_spritesheet *sheet)
    : _sheet(sheet) {
    rereadSheet();
}

SpriteSheet::SpriteSheet(CmdSpriteSheetCreate &cmd)
    : _sheet(cmd.cmd.sheet) {
    rereadSheet();
}

END_NS_GK;
