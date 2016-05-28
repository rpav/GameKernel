#include <GL/glew.h>
#include <GL/gl.h>

#include "nanovg.h"
#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/nvg.hpp"
#include "gk/log.hpp"

static void gk_process_nvg_path(gk_context *gk, gk_bundle *bundle, gk_cmd_path *cmd);
static void gk_process_nvg_text(gk_context *gk, gk_cmd_text *cmd);

void gk_process_nvg(gk_context *gk, gk_bundle *bundle, gk_list_nvg *list_nvg) {
    auto nvg = gk->nvg;
    auto list = GK_LIST(list_nvg);

    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(nvg, list_nvg->width, list_nvg->height, list_nvg->ratio);
    for(int i = 0; i < list->ncmds; ++i) {
        auto cmd = list->cmds[i];
        switch(GK_CMD_TYPE(cmd)) {
            case GK_CMD_PATH:
                gk_process_nvg_path(gk, bundle, (gk_cmd_path*)cmd);
                gk_gl_reset_state(gk);
                break;
            case GK_CMD_TEXT:
                gk_process_nvg_text(gk, (gk_cmd_text*)cmd);
                gk_gl_reset_state(gk);
                break;
            default:
                gk_process_cmd_general("GK_LIST_NVG", gk, bundle, cmd);
                break;
        }
    }
    nvgEndFrame(nvg);
}

void gk_process_nvg_path(gk_context *gk, gk_bundle *bundle, gk_cmd_path *cmd) {
    auto def = cmd->pathdef;
    auto end = def + cmd->pathlen;
    auto nvg = gk->nvg;

    for(; def < end ; ++def) {
        int id = (int)def[0];

        switch(id) {
            case GK_PATH_BEGIN:
                nvgBeginPath(nvg);
                break;

            case GK_PATH_RECT:
                nvgRect(nvg, def[1], def[2], def[3], def[4]);
                def += 4;
                break;

            case GK_PATH_CIRCLE:
                nvgCircle(nvg, def[1], def[2], def[3]);
                def += 3;
                break;

            case GK_PATH_WINDING:
                nvgPathWinding(nvg, def[1]);
                def++;
                break;

            case GK_PATH_STROKE_COLOR_RGBA:
                nvgStrokeColor(nvg, nvgRGBA(def[1], def[2], def[3], def[4]));
                def += 4;
                break;

            case GK_PATH_STROKE_COLOR_RGBAF:
                nvgStrokeColor(nvg, nvgRGBAf(def[1], def[2], def[3], def[4]));
                def += 4;
                break;

            case GK_PATH_MITER_LIMIT:
                nvgMiterLimit(nvg, def[1]);
                def++;
                break;

            case GK_PATH_STROKE_WIDTH:
                nvgStrokeWidth(nvg, def[1]);
                def++;
                break;

            case GK_PATH_LINE_CAP:
                nvgLineCap(nvg, def[1]);
                def++;
                break;

            case GK_PATH_LINE_JOIN:
                nvgLineJoin(nvg, def[1]);
                def++;
                break;

            case GK_PATH_FILL_COLOR_RGBA:
                nvgFillColor(nvg, nvgRGBA(def[1], def[2], def[3], def[4]));
                def += 4;
                break;

            case GK_PATH_FILL_COLOR_RGBAF:
                nvgFillColor(nvg, nvgRGBAf(def[1], def[2], def[3], def[4]));
                def += 4;
                break;

            case GK_PATH_CLOSE:
                nvgClosePath(nvg);
                break;

            case GK_PATH_STROKE:
                nvgStroke(nvg);
                break;

            case GK_PATH_FILL:
                nvgFill(nvg);
                break;

            case GK_PATH_TF_IDENTITY:
                nvgResetTransform(nvg);
                break;

            case GK_PATH_TF_TRANSLATE:
                nvgTranslate(nvg, def[1], def[2]);
                def += 2;
                break;

            case GK_PATH_TF_ROTATE:
                nvgRotate(nvg, def[1]);
                ++def;
                break;

            case GK_PATH_TF_SKEW_X:
                nvgSkewX(nvg, def[1]);
                ++def;
                break;

            case GK_PATH_TF_SKEW_Y:
                nvgSkewY(nvg, def[1]);
                ++def;
                break;

            default:
                LOG("GK_NVG: Don't know how to handle path command ", id);
                return;
        }
    }
}

void gk_process_nvg_font_create(gk_context *gk, gk_cmd_font_create *cmd) {
    auto nvg = gk->nvg;
    cmd->id = nvgCreateFont(nvg, cmd->name, cmd->filename);
}

void gk_process_nvg_font_face(gk_context *gk, gk_cmd_font_face* cmd) {
    auto nvg = gk->nvg;

    switch(cmd->type) {
        case GK_FONT_FACE_ID:
            nvgFontFaceId(nvg, cmd->face.id);
            break;

        case GK_FONT_FACE_NAME:
            nvgFontFace(nvg, cmd->face.name);
            break;
    }
}

void gk_process_nvg_image_create(gk_context *gk, gk_cmd_image_create *cmd) {
    static const GLenum filter2gl[] = {
        GL_NEAREST,
        GL_LINEAR,
        GL_NEAREST_MIPMAP_NEAREST,
        GL_LINEAR_MIPMAP_NEAREST,
        GL_NEAREST_MIPMAP_LINEAR,
        GL_LINEAR_MIPMAP_LINEAR
    };

    auto nvg = gk->nvg;
    int flags = cmd->flags & GK_IMAGE_FLAGS_NANOVG_MASK;
        
    cmd->id = nvgCreateImage(nvg, cmd->filename, flags);
    glBindTexture(GL_TEXTURE_2D, cmd->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter2gl[cmd->min_filter]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter2gl[cmd->mag_filter]);
}

void gk_process_nvg_font_style(gk_context *gk, gk_cmd_font_style *cmd) {
    auto nvg = gk->nvg;
    if(cmd->size > 0)        nvgFontSize(nvg, cmd->size);
    if(cmd->blur >= 0)       nvgFontBlur(nvg, cmd->blur);
    if(cmd->spacing > 0)     nvgTextLetterSpacing(nvg, cmd->spacing);
    if(cmd->line_height > 0) nvgTextLineHeight(nvg, cmd->line_height);
    if(cmd->align > 0)       nvgTextAlign(nvg, cmd->align);
}

void gk_process_nvg_text(gk_context *gk, gk_cmd_text *cmd) {
    auto nvg = gk->nvg;

    if(cmd->break_width > 0.0) {
        nvgTextBox(nvg, cmd->pos.x, cmd->pos.y,
                   cmd->break_width, cmd->str, cmd->end);
    } else {
        nvgText(nvg, cmd->pos.x, cmd->pos.y, cmd->str, cmd->end);
    }
}
