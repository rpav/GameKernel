/******************************************************************
 * Core Constants/Enums
 ******************************************************************/

#ifndef __GAMEKERNEL_CORECONST_H__
#define __GAMEKERNEL_CORECONST_H__

typedef enum gk_impl {
    GK_GL2,
    GK_GL3
} gk_impl;

typedef enum gk_cmd_type {
    GK_CMD_NULL,
    GK_CMD_PASS,
    GK_CMD_PATH,
    GK_CMD_FONT_CREATE,
    GK_CMD_FONT_STYLE,
    GK_CMD_FONT_FACE,
    GK_CMD_TEXT,
    GK_CMD_IMAGE_CREATE,

    /* Transforms */
    GK_CMD_TF_TRS,
    GK_CMD_TF_ORTHO,

    /* GL */
    GK_CMD_QUAD,
    GK_CMD_QUADSPRITE,

    /* Box2D physics */
    GK_CMD_B2_WORLD_CREATE,
    GK_CMD_B2_WORLD_DESTROY,
    GK_CMD_B2_BODY_CREATE,
    GK_CMD_B2_FIXTURE_CREATE,

    GK_CMD_B2_STEP,
    GK_CMD_B2_ITER_BODIES,

    GK_CMD_B2_DRAW_DEBUG,

    /* Misc */
    GK_CMD_SPRITESHEET_CREATE,
    GK_CMD_SPRITESHEET_DESTROY,

    /* The last core command ID */
    GK_CMD_MAX
} gk_cmd_type;

typedef enum gk_subsystem {
    GK_SUB_NULL,
    GK_SUB_CONFIG,
    GK_SUB_GL,
    GK_SUB_NVG,
    GK_SUB_BOX2D,

    /* Last core subsystem ID */
    GK_SUB_MAX
} gk_subsystem;

#endif  /* __GAMEKERNEL_CORECONST_H__ */
