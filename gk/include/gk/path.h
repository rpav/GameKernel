/******************************************************************
 * Paths
 *
 * Paths are described by a linear float array containing an "encoded"
 * series of path commands.  A path command is simple a float giving
 * the command type, followed by further floats which are parameters.
 *
 * For example:
 *
 * float pathdef[] = { GK_PATH_BEGIN,
 *                     GK_PATH_RECT, 0.0, 0.0, 10.0, 10.0,
 *                     GK_PATH_FILL_COLOR_RGBA, 1.0, 0.0, 0.0, 0.0,
 *                     GK_PATH_FILL };
 *
 * Paths may be used in a NanoVG pass to draw shapes, or a Box2D pass
 * to define fixtures, etc.
 ******************************************************************/

#ifndef __GAMEKERNEL_PATH_H__
#define __GAMEKERNEL_PATH_H__

typedef enum gk_pathdef_cmds {
    GK_PATH_BEGIN,

    /* Path */
    GK_PATH_MOVE_TO,
    GK_PATH_LINE_TO,
    GK_PATH_BEZIER_TO,
    GK_PATH_QUAD_TO,
    GK_PATH_ARC_TO,
    GK_PATH_CLOSE,

    GK_PATH_ARC,
    GK_PATH_RECT,
    GK_PATH_ROUNDED_RECT,
    GK_PATH_ELLIPSE,
    GK_PATH_CIRCLE,

    GK_PATH_WINDING,

    /* Stroke & Fill */
    GK_PATH_STROKE_COLOR_RGBA,
    GK_PATH_STROKE_COLOR_RGBAF,
    GK_PATH_MITER_LIMIT,
    GK_PATH_STROKE_WIDTH,
    GK_PATH_LINE_CAP,
    GK_PATH_LINE_JOIN,
    GK_PATH_FILL_COLOR_RGBA,
    GK_PATH_FILL_COLOR_RGBAF,

    GK_PATH_FILL,
    GK_PATH_STROKE,

    /* Transforms */
    GK_PATH_TF_IDENTITY,
    GK_PATH_TF_TRANSLATE,
    GK_PATH_TF_ROTATE,
    GK_PATH_TF_SCALE,
    GK_PATH_TF_SKEW_X,
    GK_PATH_TF_SKEW_Y,

    /* A few physicsy things */
    GK_PATH_DENSITY,
    GK_PATH_ELASTICITY,
    GK_PATH_FRICTION,

    GK_PATH_SENSOR,

    GK_PATH_CATEGORY,
    GK_PATH_CATEGORY_MASK,
    GK_PATH_GROUP,

    GK_PATH_FIXTURE_ID,

    /* Max path command */
    GK_PATH_CMD_MAX
} gk_pathdef_cmds;

/* These should correspond to NanoVG */
typedef enum gk_pathdef_winding {
    GK_WINDING_CCW = 1,
    GK_WINDING_SOLID = 1,

    GK_WINDING_CW = 2,
    GK_WINDING_HOLE = 2
} gk_pathdef_winding;

/* This should correspond to NanoVG */
typedef enum gk_pathdef_linecap {
    GK_LINECAP_BUTT,
    GK_LINECAP_ROUND,
    GK_LINECAP_SQUARE,
    GK_LINECAP_BEVEL,
    GK_LINECAP_MITER
} gk_pathdef_linecap;

/* This should precisely mask NanoVG-passed-through flags */
#define GK_TEX_FLAGS_NANOVG_MASK 0x1F

typedef struct gk_cmd_path {
    gk_cmd parent;

    float *pathdef;
    size_t pathlen;
} gk_cmd_path;

#endif /* __GAMEKERNEL_PATH_H__ */
