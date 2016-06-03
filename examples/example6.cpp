#include <GL/glew.h>
#include <GL/gl.h>
#include "gk/gk++.hpp"
#include "gk/log.hpp"
#include "example.hpp"

static const char *shader_geom_quad =
 "#version 330 core\n"

 "layout(lines_adjacency) in;"
 "layout(triangle_strip, max_vertices = 4) out;"

 "in vec2 vert_uv[];"
 "out vec2 uv;"

 "void main() {"
 "    uv = vert_uv[0];"
 "    gl_Position = gl_in[0].gl_Position;"
 "  EmitVertex();"
 "    uv = vert_uv[1];"
 "    gl_Position = gl_in[1].gl_Position;"
 "  EmitVertex();"
 "    uv = vert_uv[2];"
 "    gl_Position = gl_in[2].gl_Position;"
 "  EmitVertex();"
 "    uv = vert_uv[3];"
 "    gl_Position = gl_in[3].gl_Position;"
 "  EmitVertex();"
 
 "  EndPrimitive();"
 "}"
 ;

static const char *shader_vert_quad =
 "#version 330 core\n"

 "layout (location = 0) in vec4 vertex;"
 "layout (location = 1) in vec2 in_uv;"

 "out vec2 vert_uv;"

 "void main() {"
 "  vert_uv = in_uv;"
 "  gl_Position = vertex;"
 "}"
 ;

static const char *shader_frag_quad =
 "#version 330 core\n"

 "uniform sampler2D tex;"

 "in vec2 uv;"
 "out vec4 frag;"

 "void main() {"
 "  frag = texture(tex, uv);"
 "  frag.b = frag.b/2;"
 "  if(frag.a == 0.0) discard;"
 "}"
 ;

void example_main() {
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    auto gk = gk::create(GK_GL3);

    float width = WIDTH/8, height = HEIGHT/8;

    gk::Bundle bundle(0);
    gk::ListNvg nvg(width, height, 1.0);

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

    gk::ShaderSource geometry(GK_SHADER_GEOMETRY, shader_geom_quad);
    gk::ShaderSource vertex(GK_SHADER_VERTEX, shader_vert_quad);
    gk::ShaderSource fragment(GK_SHADER_FRAGMENT, shader_frag_quad);
    gk::ProgramSource prog;
    prog.add(geometry, vertex, fragment);

    gk::CmdProgramCreate progCreate;
    gk::CmdProgramDestroy progDestroy;
    progCreate.add(prog);

    bundle.add(config);
    config.add(rtCreate, progCreate);
    gk::process(gk, bundle);

    gk::CmdRtBind bind(rtCreate);
    gk::CmdRtDestroy rtDestroy(rtCreate);
    
    progDestroy.add(progCreate);

    LOG("framebuffer = ", bind.cmd.framebuffer, " tex = ", rtCreate.cmd.tex);

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

    gk::ListGL gl(WIDTH, HEIGHT);
    gk::CmdQuad quad(rtCreate.cmd.tex, -0.8, -0.8, 0.8, 0.8);
    quad.cmd.program = prog;

    bundle.add(gl);
    gl.add(quad, rtDestroy, progDestroy);

    gk::process(gk, bundle);
    swap();

    wait();
    gk::destroy(gk);
}
