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

    /* Transforms */
    GK_CMD_TF_TRS,
    GK_CMD_TF_ORTHO,

    /* GL */
    GK_CMD_CLEAR,
    GK_CMD_QUAD,
    GK_CMD_QUADSPRITE,

    /* Render Texture */
    GK_CMD_RT_CREATE,
    GK_CMD_RT_DESTROY,
    GK_CMD_RT_BIND,
    GK_CMD_RT_UNBIND,

    /* Shaders */
    GK_CMD_PROGRAM_CREATE,
    GK_CMD_PROGRAM_DESTROY,
    GK_CMD_UNIFORM_QUERY,

    /* NanoVG */
    GK_CMD_FONT_CREATE,
    GK_CMD_FONT_STYLE,
    GK_CMD_FONT_FACE,
    GK_CMD_PATH,
    GK_CMD_TEXT,
    GK_CMD_IMAGE_CREATE,

    /* Box2D physics */
    GK_CMD_B2_WORLD_CREATE,
    GK_CMD_B2_WORLD_DESTROY,
    GK_CMD_B2_BODY_CREATE,
    GK_CMD_B2_FIXTURE_CREATE,

    GK_CMD_B2_STEP,
    GK_CMD_B2_ITER_BODIES,

    GK_CMD_B2_DRAW_DEBUG,

    /* Spritesheet */
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

typedef enum gk_error_code {
    GK_ERROR_NONE = 0,

    /* Lists */
    GK_ERROR_LIST_RECURSION,
    GK_ERROR_LIST_UNKNOWN,
    GK_ERROR_LIST_CORRUPT,

    /* Pass */
    GK_ERROR_PASS_BADLIST,

    /* Commands */
    GK_ERROR_CMD_UNKNOWN,

    /* GL */
    GK_ERROR_COMPILING_SHADER,
    GK_ERROR_DESTROYING_PROGRAM,

    /* Spritesheet */
    GK_ERROR_SSF_UNKNOWN,

    /* End of the line */
    GK_ERROR_MAX
} gk_error_code;

#endif  /* __GAMEKERNEL_CORECONST_H__ */
