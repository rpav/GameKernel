#include <map>

#include <unistd.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "gk/gk.h"
#include <rpav/log.hpp>
#include "example.hpp"

using mat4 = gk::mat4;
using vec4 = gk::vec4;
using vec3 = gk::vec3;
using vec2 = gk::vec2;

void example_main(int, const char**) {
    gk_context *gk = gk_create(GK_GL3);
    gk_bundle b;
    gk_list_gl l;
    gk_cmd_spritesheet_create c;
    gk_cmd_spritesheet_destroy d;

    l.viewport.size.x = l.viewport.size.y = 0;

    //mat4 proj = glm::ortho<float>(0, WIDTH, HEIGHT, 0, -1, 1);
    mat4 proj = gk::mat4::ortho(0, WIDTH, 0, HEIGHT, -1, 1);

    gk_cmd_tf_trs tf;
    gk_cmd_quadsprite qs;

    init_bundle(&b, 0, 1);
    b.lists[0] = GK_LIST(&l);

    init_list(GK_LIST(&l), GK_SUB_CONFIG, 2);
    l.parent.ncmds = 1;
    l.parent.cmds[0] = GK_CMD(&c);

    GK_CMD_TYPE(&c) = GK_CMD_SPRITESHEET_CREATE;
    //c.flags = 0;
    c.flags = GK_SCF_FLIP_Y;
    c.path = "res/platformer_sprites.json";
    c.format = GK_SSF_TEXTUREPACKER_JSON;

    gk_process(gk, &b);

    // Find our animation
    gk_sprite_id f_min = 0, f_max = 0;
    std::map<std::string,int> frames;

    for(size_t i = 0; i < c.sheet->nsprites; ++i)
        frames[c.sheet->names[i]] = i;

    // Note this assumes these are in order; in our case they are, but
    // at least with TexturePacker they must be named alphabetically,
    // e.g. 'foo_10' comes before 'foo_2'.
    f_min = frames["walk_0.png"];
    f_max = frames["walk_7.png"];
    say("Animating indexes ", f_min, " - ", f_max);

    // Draw
    auto *sheet = c.sheet;

    GK_CMD_TYPE(&tf) = GK_CMD_TF_TRS;
    tf.parent.prior = (gk_mat4*)&proj;
    tf.parent.out = &qs.tfm;
    tf.flags = 0;
    tf.translate = {0, 0, 0};


    GK_CMD_TYPE(&qs) = GK_CMD_QUADSPRITE;
    qs.sheet = sheet;
    qs.index = f_min;
    qs.pds = nullptr;

    l.parent.sub = GK_SUB_GL;
    l.parent.ncmds = 2;
    l.parent.cmds[0] = GK_CMD(&tf);
    l.parent.cmds[1] = GK_CMD(&qs);

    do {
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);
        gk_process(gk, &b);
        swap();

        qs.index++;

        if(qs.index > f_max)
            qs.index = f_min;

        usleep(120000);
    } while(!check_input());

    // Destroy the sheet.
    GK_CMD_TYPE(&d) = GK_CMD_SPRITESHEET_DESTROY;
    gk_spritesheet* sheets[] = { c.sheet };
    d.nsheets = 1;
    d.sheets = sheets;
    l.parent.sub = GK_SUB_CONFIG;
    l.parent.ncmds = 1;
    l.parent.cmds[0] = GK_CMD(&d);

    gk_process(gk, &b);
}
