#include <unistd.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using mat4 = glm::mat4;
using vec4 = glm::vec4;
using vec3 = glm::vec3;
using vec2 = glm::vec2;

#include <vector>
#include <random>
#include <chrono>
#include <functional>

#include "gk/gk.h"
#include "gk/log.hpp"
#include "example.hpp"

const int MAXCMD = 5000;

struct CmdVector {
    size_t size;
    gk_cmd **vector;
    size_t fill;

    CmdVector(size_t size_) {
        size = size_;
        vector = new gk_cmd*[size];
        fill = 0;
    }

    void add(gk_cmd *cmd) {
        if(fill >= size) {
            LOG("CmdVector overflow!");
            return;
        }
        vector[fill] = cmd;
        ++fill;
    }

    void reset() {
        fill = 0;
    }
};

CmdVector Cmds(MAXCMD);

class Sprite {
    gk_cmd_quad _cmd;
public:
    gk_cmd_tf_trs tf;
    vec2 size;
    float scale;
    float scale_delta;

    int tex;

    Sprite(int key, int tex_, vec3 pos, vec2 size_, float rotate = 0.0f, float scale_ = 1.0f) {
        tex = tex_;
        size = size_;
        scale = scale_;

        tf.parent.prior = nullptr;
        tf.flags = GK_TRS_SCALE;
        tf.translate.set(glm::value_ptr(pos));
        tf.angle = rotate;
        tf.axis.set(0, 0, 1);
        tf.parent.out = &_cmd.tfm;

        scale_delta = 0.01;

        GK_CMD_TYPE(&_cmd) = GK_CMD_QUAD;
        GK_CMD_TYPE(&tf) = GK_CMD_TF_TRS;

        GK_CMD_KEY(&tf) = key;
        GK_CMD_KEY(&_cmd) = key+5000;

        _cmd.tex = tex;
        _cmd.pds = nullptr;

        _cmd.attr[0].vertex.set(-0.5, -0.5, 0, 1);
        _cmd.attr[1].vertex.set( 0.5, -0.5, 0, 1);
        _cmd.attr[2].vertex.set(-0.5,  0.5, 0, 1);
        _cmd.attr[3].vertex.set( 0.5,  0.5, 0, 1);

        _cmd.attr[0].uv.set(0, 1);
        _cmd.attr[1].uv.set(1, 1);
        _cmd.attr[2].uv.set(0, 0);
        _cmd.attr[3].uv.set(1, 0);
    }

    void draw(const mat4 &m) {
        tf.parent.prior = (gk_mat4*)&m;
        tf.scale.set(scale*size.x, scale*size.y, 1);
        Cmds.add((gk_cmd*)&tf);
        Cmds.add((gk_cmd*)&_cmd);
    }
};

void example_main() {
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    gk_context *gk = gk_create(GK_GL3);

    gk_bundle bundle_cfg;
    gk_list list_cfg;
    gk_cmd_image_create img_create;

    gk_bundle bundle_render;
    gk_list_gl list_spr;
    list_spr.width = list_spr.height = 0;

    /* Configure/load image */
    init_bundle(&bundle_cfg, 0, 1);
    bundle_cfg.lists[0] = &list_cfg;

    init_list(&list_cfg, GK_SUB_CONFIG, 1);
    list_cfg.cmds[0] = GK_CMD(&img_create);

    GK_CMD_TYPE(&img_create) = GK_CMD_IMAGE_CREATE;
    img_create.filename = "../examples/res/test.png";
    img_create.flags = 0;
    img_create.min_filter = GK_TEX_FILTER_LINEAR;
    img_create.mag_filter = GK_TEX_FILTER_LINEAR;

    gk_process(gk, &bundle_cfg);
    free_list(&list_cfg);
    free_bundle(&bundle_cfg);
    LOG("img_create.id = ", img_create.id);

    /* Render sprites */
    init_bundle(&bundle_render, 0, 1);
    bundle_render.start.sort = GK_PASS_SORT_ASC;
    bundle_render.lists[0] = GK_LIST(&list_spr);

    mat4 proj = glm::ortho<float>(0, WIDTH, 0, HEIGHT, -1, 1);
    mat4 mvp = proj;

    printm(proj);

    const int spritesize = 64;

    typedef std::chrono::high_resolution_clock clock;
    std::default_random_engine reng;
    reng.seed(clock::now().time_since_epoch().count());
    auto r = std::bind(std::uniform_real_distribution<float>(0,1), reng);

    const int maxsprites = 1000;
    Sprite *sprites[maxsprites];
    for(int i = 0; i < maxsprites; ++i) {
        auto x = r()*WIDTH;
        auto y = r()*HEIGHT;
        sprites[i] = new Sprite(i, img_create.id, vec3(x, y, 0), vec2(spritesize, spritesize));
    }

    FPS fps;
    fps.start();

    while(!check_input()) {
        Cmds.reset();
        glClear(GL_COLOR_BUFFER_BIT);
        for(int i = 0; i < maxsprites; ++i) {
            auto sprite = sprites[i];
            sprite->tf.translate.set(r()*WIDTH, r()*HEIGHT, 0);

            /*
            sprite->tf.angle = sprite->tf.angle + (M_PI/100);
            auto scale = sprite->scale;
            if(scale >= 1.5)
                sprite->scale_delta = -0.01;
            else if(scale < 0.5)
                sprite->scale_delta = 0.01;
            sprite->scale += sprite->scale_delta;
            */

            sprite->draw(mvp);
        }
        init_list(GK_LIST(&list_spr), GK_SUB_GL, Cmds.fill, Cmds.vector);
        gk_process(gk, &bundle_render);
        swap();
        fps.frame();
    }

    for(int i = 0; i < maxsprites; ++i) {
        delete sprites[i];
    }
    gk_destroy(gk);
}
