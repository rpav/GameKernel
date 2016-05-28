/******************************************************************
 * GameKernel
 *
 * This is the only header you need to include.  Note that while some
 * dependencies are optional, various support structs and constants
 * are not optionalized in this header.  Simply don't use what you
 * don't want; it will otherwise not exist at runtime.
 ******************************************************************/


#ifndef __GAMEKERNEL_CAPI_H__
#define __GAMEKERNEL_CAPI_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************
 * Enums
 ******************************************************************/
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


/******************************************************************
 * Basic structures
 ******************************************************************/
#ifdef __cplusplus
#define CXX_SET(...) void set(__VA_ARGS__);
#else
#define CXX_SET(...)
#endif

typedef struct gk_vec2 {
    float x, y;

    CXX_SET(float x, float y)
    CXX_SET(float*)
} gk_vec2;

typedef struct gk_vec3 {
    float x, y, z;

    CXX_SET(float x, float y, float z)
    CXX_SET(float*)
} gk_vec3;

typedef struct gk_vec4 {
    float x, y, z, w;

    CXX_SET(float x, float y, float z, float w)
    CXX_SET(float*)
} gk_vec4;

typedef union gk_mat4 {
    float data[16];

    struct {
        float a00, a01, a02, a03;
        float a10, a11, a12, a13;
        float a20, a21, a22, a23;
        float a30, a31, a32, a33;
    } v;

    CXX_SET(float*)
} gk_mat4;

typedef struct gk_cmd {
    gk_cmd_type type;
    int32_t key;
} gk_cmd;

#define GK_CMD(x) ((gk_cmd*)(x))
#define GK_CMD_TYPE(x) (GK_CMD(x)->type)
#define GK_CMD_KEY(x) (GK_CMD(x)->key)

typedef struct gk_list {
    gk_subsystem sub;

    size_t ncmds;
    gk_cmd **cmds;

    /* Internal */
    bool in_use;
} gk_list;

#define GK_LIST(x) ((gk_list*)(x))

typedef enum gk_pass_sorting {
    GK_PASS_SORT_NONE,
    GK_PASS_SORT_ASC,
    GK_PASS_SORT_DESC,
    GK_PASS_SORT_STABLE_ASC,
    GK_PASS_SORT_STABLE_DESC
} gk_pass_sorting;

typedef struct gk_pass {
    gk_cmd parent;
    gk_pass_sorting sort;
    size_t list_index;
} gk_pass;

#define GK_PASS(x) ((gk_pass*)(x))
#define GK_SUBSYSTEM(x) (GK_PASS(x)->type)

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

    /* Spritesheet */
    GK_ERROR_SSF_UNKNOWN,

    /* End of the line */
    GK_ERROR_MAX
} gk_error_code;

typedef struct gk_error {
    gk_error_code code;
    const char *message;
} gk_error;

typedef struct gk_bundle {
    gk_pass start;
    gk_list **lists;
    size_t nlists;

    gk_error error;
} gk_bundle;

typedef struct gk_context gk_context;


/******************************************************************
 * API
 ******************************************************************/
gk_context* gk_create(gk_impl impl);
void gk_destroy(gk_context *gk);
void gk_process(gk_context *gk, gk_bundle *bundle);

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
 * Paths may be used in a NanoVG pass to draw shapes.
 ******************************************************************/
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
    GK_PATH_TF_SKEW_Y
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

typedef enum gk_image_flags {
    /* These should correspond to NanoVG */
    GK_IMG_GENERATE_MIPMAPS = 1 << 0,
    GK_IMG_REPEATX          = 1 << 1,
    GK_IMG_REPEATY          = 1 << 2,
    GK_IMG_FLIPY            = 1 << 3,
    GK_IMG_PREMULTIPLIED    = 1 << 4,
} gk_image_flags;

/* This should precisely mask NanoVG-passed-through flags */
#define GK_IMAGE_FLAGS_NANOVG_MASK 0x1F

typedef enum gk_image_filter {
    /* These do _not_ numerically correspond to GLenum */
    GK_IMG_FILTER_NEAREST,
    GK_IMG_FILTER_LINEAR,
    GK_IMG_FILTER_NEAREST_MIPMAP_NEAREST,
    GK_IMG_FILTER_LINEAR_MIPMAP_NEAREST,
    GK_IMG_FILTER_NEAREST_MIPMAP_LINEAR,
    GK_IMG_FILTER_LINEAR_MIPMAP_LINEAR
} gk_image_filter;

typedef struct gk_cmd_path {
    gk_cmd parent;

    float *pathdef;
    size_t pathlen;
} gk_cmd_path;

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
    gk_image_filter min_filter;
    gk_image_filter mag_filter;

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

/******************************************************************
 * Transforms
 ******************************************************************/
typedef struct gk_cmd_tf {
    gk_cmd parent;
    gk_mat4 *prior;
    gk_mat4 *out;
} gk_cmd_tf;

typedef enum gk_trs_flags {
    /* Translation always happens */
    GK_TRS_ROTATE    = 1 << 0,
    GK_TRS_SCALE     = 1 << 1
} gk_trs_flags;

typedef struct gk_cmd_tf_trs {
    gk_cmd_tf parent;

    uint32_t flags;

    gk_vec3 translate;
    gk_vec3 scale;
    gk_vec3 axis;
    float angle;
} gk_cmd_tf_trs;

typedef struct gk_cmd_tf_ortho {
    /* Prior is currently ignored for tf_ortho */
    gk_cmd_tf parent;

    float left, right, bottom, top;
    float near, far;
} gk_cmd_tf_ortho;

/******************************************************************
 * GL
 ******************************************************************/
typedef struct gk_list_gl {
    gk_list parent;
} gk_list_gl;

typedef struct gk_quadvert {
    gk_vec4 vertex;
    gk_vec2 uv;
} gk_quadvert;

typedef struct gk_cmd_quad {
    gk_cmd parent;

    /* The texture name, not the bound texture number */
    int tex;
    gk_mat4 tfm;
    gk_quadvert attr[4];
} gk_cmd_quad;

/* Spritesheet support */
typedef struct gk_sprite {
    gk_quadvert attr[4]; /* Precalculated vertices/UVs */

    /* These are for reference */
    gk_vec2 size;    /* The size of the rectangle in the texture */
    gk_vec2 vsize;   /* The "virtual" size of the sprite, which may be larger */
    gk_vec3 anchor;  /* The original anchor, relative to the "virtual" size */
} gk_sprite;

typedef struct gk_spritesheet {
    unsigned int tex;           /* Texture for the sprites */

    size_t nsprites;
    gk_sprite *sprites;
    char **names;               /* Array of (nsprites) strings naming
                                   each correspondingly-indexed sprite */
} gk_spritesheet;

typedef struct gk_cmd_quadsprite {
    gk_cmd parent;

    gk_mat4 tfm;
    gk_spritesheet *sheet;
    unsigned int index;
} gk_cmd_quadsprite;


/******************************************************************
 * Box2D
 ******************************************************************/
typedef struct gk_b2_world {
    float timestep;
    int velocity_iterations;
    int position_iterations;

    /* Internal data */
    void *data;
} gk_b2_world;

/* These should correspond to b2BodyType */
typedef enum gk_b2_body_type {
    GK_B2_BODY_TYPE_STATIC = 0,
    GK_B2_BODY_TYPE_KINEMATIC,
    GK_B2_BODY_TYPE_DYNAMIC
} gk_b2_body_type;

typedef struct gk_b2_body {
    /* Internal */
    void *data;
} gk_b2_body;

typedef struct gk_b2_bodydef {
    gk_b2_body_type type;

    gk_vec2 position;
    gk_vec2 linear_velocity;
    float linear_damping;

    float angle;
    float angular_velocity;
    float angular_damping;

    float gravity_scale;

    bool active;
    bool awake;
    bool bullet;
    bool fixed_rotation;
    bool no_sleep;              /* Opposite of b2BodyDef.allowSleep */

    /* Provide this */
    gk_b2_body *body;
} gk_b2_bodydef;

typedef struct gk_cmd_b2_world_create {
    gk_cmd parent;

    /* Provide this */
    gk_b2_world *world;

    /* Settings */
    gk_vec2 gravity;
    char do_sleep;
} gk_cmd_b2_world_create;

typedef struct gk_cmd_b2_world_destroy {
    gk_cmd parent;

    /* Provide this */
    gk_b2_world *world;
} gk_cmd_b2_world_destroy;

typedef struct gk_cmd_b2_body_create {
    gk_cmd parent;

    gk_b2_world *world;

    size_t ndefs;
    gk_b2_bodydef **defs;
} gk_cmd_b2_body_create;

typedef struct gk_cmd_b2_fixture_create {
    gk_cmd parent;

    gk_b2_body *body;

    /* Fixtures are created by specifying paths; e.g., GK_PATH_RECT
       makes a b2PolygonShape, GK_PATH_CIRCLE makes a b2CircleShape,
       etc. Arcs, bezier curves, etc are not supported at all.

       End shapes with GK_PATH_FILL.

       Ending a shape with GK_PATH_STROKE will create a b2ChainShape;
       note that only GK_PATH_LINE_TO is supported for this.
       GK_PATH_CLOSE will produce a closed chain.
    */
    size_t pathlen;
    float *pathdef;
} gk_cmd_b2_fixture_create;


/******************************************************************
 * Spritesheet loading
 *
 * Note this isn't necessary to use spritesheets; you can fill out
 * and manage your own gk_spritesheet structs.
 ******************************************************************/
typedef enum gk_spritesheet_format {
    GK_SSF_TEXTUREPACKER_JSON
} gk_spritesheet_format;

typedef enum gk_spritesheet_create_flags {
    /* These flip the "direction" of coordinates on the sprite.  This
       is useful if you prefer your X/Y to increase in the opposite
       direction of your sprite sheet.  Anchors remain in the same
       "conceptual" location.
    */
    GK_SCF_FLIP_X = 1 << 0,
    GK_SCF_FLIP_Y = 1 << 1,

    /* Sprites are not mipmapped; if one of these is set, it is used
       instead of the opposing default. */
    GK_SCF_MIN_NEAREST = 1 << 2,
    GK_SCF_MAG_LINEAR  = 1 << 3,
} gk_spritesheet_create_flags;

typedef struct gk_cmd_spritesheet_create {
    gk_cmd parent;

    gk_spritesheet_format format;
    const char *path;
    const char *filename;

    uint32_t flags;

    /* Output; the returned pointer should be provided to a
       gk_cmd_spritesheet_destroy to free. */
    gk_spritesheet *sheet;
} gk_cmd_spritesheet_create;

typedef struct gk_cmd_spritesheet_destroy {
    gk_cmd parent;
    gk_spritesheet *sheet;
} gk_cmd_spritesheet_destroy;

#ifdef __cplusplus
}
#endif

#endif /* __GAMEKERNEL_CAPI_H__ */
