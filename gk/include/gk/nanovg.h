#ifndef __GAMEKERNEL_NVG_H__
#define __GAMEKERNEL_NVG_H__

/******************************************************************
 * NanoVG
 ******************************************************************/
typedef struct gk_list_nvg {
    gk_list parent;

    int width;
    int height;
    float ratio;
} gk_list_nvg;

typedef struct gk_cmd_font_create {
    gk_cmd parent;

    const char *name;
    const char *filename;

    /* This will be set */
    int id;
} gk_cmd_font_create;

typedef struct gk_cmd_image_create {
    gk_cmd parent;

    const char *filename;
    uint32_t flags;
    gk_tex_filter min_filter;
    gk_tex_filter mag_filter;

    /* This will be set */
    int id;
} gk_cmd_image_create;

/* These should correspond with NanoVG */
typedef enum gk_align {
    GK_ALIGN_LEFT     = 1 << 0,
    GK_ALIGN_CENTER   = 1 << 1,
    GK_ALIGN_RIGHT    = 1 << 2,
    GK_ALIGN_TOP      = 1 << 3,
    GK_ALIGN_MIDDLE   = 1 << 4,
    GK_ALIGN_BOTTM    = 1 << 5,
    GK_ALIGN_BASELINE = 1 << 6
} gk_align;

typedef struct gk_cmd_font_style {
    gk_cmd parent;

    float size;
    float blur;
    float spacing;
    float line_height;
    int align;
} gk_cmd_font_style;

typedef enum gk_font_face {
    GK_FONT_FACE_ID,
    GK_FONT_FACE_NAME
} gk_font_face;

typedef struct gk_cmd_font_face {
    gk_cmd parent;

    gk_font_face type;
    union {
        char *name;
        int id;
    } face;
} gk_cmd_font_face;

typedef struct gk_cmd_text {
    gk_cmd parent;

    gk_vec2 pos;

    /* If this is zero or negative, lines will not be broken */
    float break_width;
    const char *str;
    const char *end;
} gk_cmd_text;

#endif /* __GAMEKERNEL_NVG_H__ */
