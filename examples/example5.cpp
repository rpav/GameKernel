#include <GL/glew.h>
#include <GL/gl.h>
#include "gk/gk++.hpp"
#include <rpav/log.hpp>
#include "example.hpp"


void example_main(int, const char**) {
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    auto gk = gk::Context{GK_GL3};

    int32_t width = WIDTH/8, height = HEIGHT/8;

    gk::Bundle bundle(0);
    gk::ListNvg nvg({0, 0, width, height});

    gk::CmdClear clear(0, 0, 0);
    gk::CmdPath path;
    float x = 0, y = 0;
    gk::PathDef pathdef = {
        GK_PATH_BEGIN,
        GK_PATH_RECT, x, y, 120, 30,
        GK_PATH_CIRCLE, x+20, y+20, 5,
        GK_PATH_WINDING, GK_WINDING_HOLE,
        GK_PATH_FILL_COLOR_RGBA, 255, 192, 0, 255,
        GK_PATH_FILL,
        GK_PATH_FILL_COLOR_RGBA, 0, 0, 255, 255
    };

    gk::List config;

    gk::CmdRtCreate rtCreate(width, height);
    gk::CmdRtUnbind unbind;

    rtCreate.cmd.tex_min_filter = GK_TEX_FILTER_NEAREST;
    rtCreate.cmd.tex_mag_filter = GK_TEX_FILTER_NEAREST;

    bundle.add(config);
    config.add(rtCreate);
    gk.process(bundle);

    gk::CmdRtBind bind(rtCreate);
    gk::CmdRtDestroy rtDestroy(rtCreate);

    say("framebuffer = ", bind.cmd.framebuffer, " tex = ", rtCreate.cmd.tex);

    gk::List passes;
    gk::CmdPass passNvg(1);
    gk::CmdPass passGL(2);
    passes.add(passNvg);
    passes.add(passGL);

    bundle.clear();
    bundle.add(passes, nvg);

    nvg.add(bind, clear, path);

    path.setPath(pathdef);

    gk::CmdFontCreate fc("black_chancery", "../examples/res/black_chancery.ttf");
    gk::CmdFontStyle fs(40);
    gk::CmdText text("hello world", 0, height/2);

    fs.cmd.align = GK_ALIGN_LEFT | GK_ALIGN_TOP;

    nvg.add(fc, fs, text);
    nvg.add(unbind);

    gk::ListGL gl({0, 0, WIDTH, HEIGHT});
    gk::CmdQuad quad(rtCreate.cmd.tex, -0.8, -0.8, 0.8, 0.8);

    bundle.add(gl);
    gl.add(quad, rtDestroy);

    gk.process(bundle);
    swap();

    wait();
}
