#include <GL/glew.h>

#include <GL/gl.h>
#include <rpav/log.hpp>

#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/nvg.hpp"
#include "nanovg.h"

using namespace rpav;

void ensure_nvg_inframe(gk_context* gk, int w, int h, float r)
{
    if(gk->nvg_inframe) return;

    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);
    nvgBeginFrame(gk->nvg, float(w), float(h), r);
    gk->nvg_inframe = true;
}

void ensure_nvg_outframe(gk_context* gk)
{
    if(!gk->nvg_inframe) return;

    nvgEndFrame(gk->nvg);
    gk->nvg_inframe = false;
    glDisable(GL_STENCIL_TEST);
    gk_gl_reset_state(gk);
}

void gk_process_nvg_path(gk_context* gk, gk_bundle*, gk_list_nvg* list, gk_cmd_path* cmd)
{
    auto def = cmd->pathdef;
    auto end = def + cmd->pathlen;
    auto nvg = gk->nvg;

    float _Y = (list->origin == GK_ORIGIN_Y_DOWN ? 1.0 : -1.0);

    nvgSave(nvg);

    if(_Y < 0.0) {
        nvgTranslate(nvg, 0.0, list->viewport.size.y);
    }

    for(; def < end; ++def) {
        int id = (int)def[0];

        switch(id) {
            case GK_PATH_BEGIN:
                nvgBeginPath(nvg);
                break;

            case GK_PATH_SAVE:
                nvgSave(nvg);
                break;

            case GK_PATH_RESTORE:
                nvgRestore(nvg);
                break;

            case GK_PATH_MOVE_TO:
                nvgMoveTo(nvg, def[1], _Y * def[2]);
                def += 2;
                break;

            case GK_PATH_LINE_TO:
                nvgLineTo(nvg, def[1], _Y * def[2]);
                def += 2;
                break;

            case GK_PATH_RECT:
                nvgRect(nvg, def[1], _Y * def[2], def[3], _Y * def[4]);
                def += 4;
                break;

            case GK_PATH_CIRCLE:
                nvgCircle(nvg, def[1], _Y * def[2], def[3]);
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

            case GK_PATH_FONT_SIZE:
                nvgFontSize(nvg, def[1]);
                ++def;
                break;

            case GK_PATH_FONT_BLUR:
                nvgFontBlur(nvg, def[1]);
                ++def;
                break;

            case GK_PATH_FONT_SPACING:
                nvgTextLetterSpacing(nvg, def[1]);
                ++def;
                break;

            case GK_PATH_FONT_LINE_HEIGHT:
                nvgTextLineHeight(nvg, def[1]);
                ++def;
                break;

            case GK_PATH_FONT_ALIGN:
                nvgTextAlign(nvg, def[1]);
                ++def;
                break;

            case GK_PATH_TF_IDENTITY:
                nvgResetTransform(nvg);
                break;

            case GK_PATH_TF_TRANSLATE:
                nvgTranslate(nvg, def[1], _Y * def[2]);
                def += 2;
                break;

            case GK_PATH_TF_ROTATE:
                nvgRotate(nvg, _Y * def[1]);
                ++def;
                break;

            case GK_PATH_TF_SCALE:
                // Do not negate scaling with _Y
                nvgScale(nvg, def[1], def[2]);
                def += 2;
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
                say("GK_NVG: Don't know how to handle path command ", id);
                return;
        }
    }

    nvgRestore(nvg);
}

void gk_process_nvg_font_create(gk_context* gk, gk_cmd_font_create* cmd)
{
    auto nvg = gk->nvg;
    cmd->id  = nvgCreateFont(nvg, cmd->name, cmd->filename);
}

void gk_process_nvg_font_face(gk_context* gk, gk_cmd_font_face* cmd)
{
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

void gk_process_nvg_image_create(gk_context* gk, gk_cmd_image_create* cmd)
{
    auto nvg   = gk->nvg;
    int  flags = cmd->flags & GK_TEX_FLAGS_NANOVG_MASK;

    cmd->id = nvgCreateImage(nvg, cmd->filename, flags);
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, cmd->id));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                             gk_filter_to_gl[cmd->min_filter]));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                             gk_filter_to_gl[cmd->mag_filter]));
    return;

gl_error:
    return;
}

void gk_process_nvg_image_destroy(gk_context* gk, gk_cmd_image_destroy* cmd)
{
    auto nvg = gk->nvg;

    for(size_t i = 0; i < cmd->nids; ++i) nvgDeleteImage(nvg, cmd->ids[i]);
}

void gk_process_nvg_font_style(gk_context* gk, gk_cmd_font_style* cmd)
{
    auto nvg = gk->nvg;
    if(cmd->size > 0) nvgFontSize(nvg, cmd->size);
    if(cmd->blur >= 0) nvgFontBlur(nvg, cmd->blur);
    if(cmd->spacing > 0) nvgTextLetterSpacing(nvg, cmd->spacing);
    if(cmd->line_height > 0) nvgTextLineHeight(nvg, cmd->line_height);
    if(cmd->align > 0) nvgTextAlign(nvg, cmd->align);
}

void gk_process_nvg_text(gk_context* gk, gk_list_nvg* list, gk_cmd_text* cmd)
{
    auto nvg = gk->nvg;

    float _Y = (list->origin == GK_ORIGIN_Y_DOWN ? 1.0 : -1.0);

    nvgSave(nvg);

    if(_Y < 0.0) {
        nvgTranslate(nvg, 0.0, -list->viewport.size.y);
    }

    if(cmd->break_width > 0.0) {
        nvgTextBox(nvg, cmd->pos.x, _Y * cmd->pos.y, cmd->break_width, cmd->str,
                   cmd->end);
    } else {
        nvgText(nvg, cmd->pos.x, _Y * cmd->pos.y, cmd->str, cmd->end);
    }

    nvgRestore(nvg);
}
