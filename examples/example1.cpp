#include <GL/glew.h>
#include <GL/gl.h>

#undef GK_EXPORT
#include "gk/gk.h"
#include <rpav/log.hpp>
#include "example.hpp"

using namespace rpav;

void example_main() {
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    gk_context *gk = gk_create(GK_GL3);
    gk_bundle bundle;
    gk_list_nvg nvg;
    gk_list *list = GK_LIST(&nvg);
    gk_cmd_path path;
    gk_cmd_font_create font_create;
    gk_cmd_font_style font_style;
    gk_cmd_text text;

    float pathdef[] = { GK_PATH_BEGIN,
                        GK_PATH_RECT, 100, 100, 120, 30,
                        GK_PATH_CIRCLE, 120, 120, 5,
                        GK_PATH_WINDING, GK_WINDING_HOLE,
                        GK_PATH_FILL_COLOR_RGBA, 255, 192, 0, 255,
                        GK_PATH_FILL,
                        GK_PATH_FILL_COLOR_RGBA, 0, 0, 255, 255 };

    nvg.width = 1280;
    nvg.height = 720;
    nvg.ratio = 1.0;

    init_bundle(&bundle, 0, 1);
    bundle.lists[0] = list;

    init_list(list, GK_SUB_NVG, 4);
    list->cmds[0] = GK_CMD(&path);
    list->cmds[1] = GK_CMD(&font_create);
    list->cmds[2] = GK_CMD(&font_style);
    list->cmds[3] = GK_CMD(&text);

    init_path(&path, pathdef, sizeof(pathdef)/sizeof(float));

    GK_CMD_TYPE(&font_create) = GK_CMD_FONT_CREATE;
    font_create.name = "black_chancery";
    font_create.filename = "../examples/res/black_chancery.ttf";

    GK_CMD_TYPE(&font_style) = GK_CMD_FONT_STYLE;
    font_style.size = 80;
    font_style.blur = 0.0;
    font_style.spacing = 1.0;
    font_style.line_height = 1.5;
    font_style.align = GK_ALIGN_LEFT | GK_ALIGN_TOP;

    GK_CMD_TYPE(&text) = GK_CMD_TEXT;
    text.pos.x = 100;
    text.pos.y = 200;
    text.break_width = 0;
    text.str = "hello world";
    text.end = nullptr;

    gk_process(gk, &bundle);
    swap();

    free_list(list);
    free_bundle(&bundle);

    wait();
    gk_destroy(gk);
}
