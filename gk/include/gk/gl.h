/******************************************************************
 * GL ... raw GL-related stuff including quad and sprite batching.
 * Well, entirely that at the moment.  But this is where Render
 * Texture, 3D, custom shaders, etc will end up.
 *
 * Unless specifically stated, enums and constants do _not_ correspond
 * to GLenum, even if they have similar names and functions.
 ******************************************************************/

#ifndef __GAMEKERNEL_GL_H__
#define __GAMEKERNEL_GL_H__

typedef enum gk_tex_flags {
    /* These should correspond to NanoVG */
    GK_TEX_GENERATE_MIPMAPS = 1 << 0,
    GK_TEX_REPEATX          = 1 << 1,
    GK_TEX_REPEATY          = 1 << 2,
    GK_TEX_FLIPY            = 1 << 3,
    GK_TEX_PREMULTIPLIED    = 1 << 4,
} gk_tex_flags;

typedef enum gk_tex_filter {
    GK_TEX_FILTER_NEAREST,
    GK_TEX_FILTER_LINEAR,
    GK_TEX_FILTER_NEAREST_MIPMAP_NEAREST,
    GK_TEX_FILTER_LINEAR_MIPMAP_NEAREST,
    GK_TEX_FILTER_NEAREST_MIPMAP_LINEAR,
    GK_TEX_FILTER_LINEAR_MIPMAP_LINEAR
} gk_tex_filter;

typedef struct gk_list_gl {
    gk_list parent;

    /* If nonzero, set the viewport */
    float width, height;
} gk_list_gl;

typedef unsigned int gk_texture;
typedef unsigned int gk_program;
typedef int          gk_uniform;

typedef struct gk_program_data_set gk_program_data_set;

/* Clear ************************************************************/

typedef enum gk_clear_flags {
    GK_CLEAR_COLOR   = 1 << 0,
    GK_CLEAR_DEPTH   = 1 << 1,
    GK_CLEAR_STENCIL = 1 << 2
} gk_clear_flags;

typedef struct gk_cmd_clear {
    gk_cmd cmd;

    uint32_t flags;

    gk_vec4 color;
    double  depth;   /* GL defaults to 1 */
    int     stencil; /* GL defaults to 0 */
} gk_cmd_clear;

/* Quads ************************************************************/

typedef struct gk_quadvert {
    gk_vec4 vertex;
    gk_vec2 uv;
} gk_quadvert;

typedef struct gk_cmd_quad {
    gk_cmd parent;

    /* The texture name, not the bound texture number */
    gk_texture tex;

    /* Program Data Set.  If this is NULL, the default will be used */
    gk_program_data_set* pds;

    gk_mat4     tfm;
    gk_quadvert attr[4];
} gk_cmd_quad;

/* Spritesheet support.  Note this is not specific to any format, nor
   are you required to use a loader. */
typedef struct gk_sprite {
    gk_quadvert attr[4]; /* Precalculated vertices/UVs */

    /* These are for reference */
    gk_vec2 size;   /* The size of the rectangle in the texture */
    gk_vec2 vsize;  /* The "virtual" size of the sprite, which may be larger */
    gk_vec3 anchor; /* The original anchor, relative to the "virtual" size */
} gk_sprite;

typedef uint32_t gk_sprite_id;

typedef struct gk_spritesheet {
    gk_texture tex; /* Texture for the sprites */

    size_t     nsprites;
    gk_sprite* sprites;

    /* Array of (nsprites) strings naming each correspondingly-indexed
       sprite.  This is filled in by loaders, but otherwise not used
       internally. */
    char** names;
} gk_spritesheet;

typedef struct gk_cmd_quadsprite {
    gk_cmd parent;

    /* Program Data Set.  If this is NULL, the default will be used */
    gk_program_data_set* pds;

    gk_mat4         tfm;
    gk_spritesheet* sheet;
    gk_sprite_id    index;
} gk_cmd_quadsprite;

typedef enum gk_spritelayer_flags {
    GK_SPRITELAYER_FLIPX = 1 << 0,
    GK_SPRITELAYER_FLIPY = 1 << 1,
} gk_spritelayer_flags;

typedef struct gk_spritelayer_config {
    gk_spritesheet* sheet;

    gk_vec2 layer_size;  // Integer WxH of sprites
    gk_vec2 sprite_size; // Uniform sprite size
} gk_spritelayer_config;

typedef struct gk_spritelayer_render {
    gk_program_data_set* pds; // Null means default

    gk_mat4  tfm;    // Transform for the entire layer
    gk_vec4  bounds; // x,y w,h for how much to render
    uint32_t flags;  // gk_spritelayer_flags
} gk_spritelayer_render;

typedef struct gk_cmd_spritelayer {
    gk_cmd parent;

    // Data for rendering etc; many layers can link one config
    gk_spritelayer_config* config;
    gk_spritelayer_render* render;

    // Provide this, a w*h array of sprite indexes for sheet
    gk_sprite_id* sprites;
} gk_cmd_spritelayer;

/* Spritechunks in a chunklayer are all the same size, as
   specified in the layer.  They do not have individual
   transform, render, etc.
*/
typedef struct gk_spritechunk {
    gk_ivec2      offset;
    gk_sprite_id* sprites;
} gk_spritechunk;

typedef struct gk_chunklayer_config {
    gk_spritesheet* sheet;

    gk_ivec2 chunk_size;  // WxH of each chunk in sprites
    gk_vec2  sprite_size; // Uniform sprite size in pixels
} gk_chunklayer_config;

typedef struct gk_cmd_chunklayer {
    gk_cmd cmd;

    gk_chunklayer_config*  config;
    gk_spritelayer_render* render;

    // What the lower-left corner of the chunk array represents
    gk_vec2 origin;
    gk_mat4 tfm; // Secondary transform

    /*
      This should be an array of chunks of size `nchunks`.  Each chunk specifies its offset
      from the origin.
    */
    size_t          nchunks;
    gk_spritechunk* chunks;
} gk_cmd_chunklayer;

/* Render Texture ***************************************************/

/* While this is a "GL" thing, you may set up and start/end RT
   anywhere (e.g., in a CONFIG list).  This can wrap an NVG list to
   draw to a texture for instance.  Note that NVG requires a stencil
   buffer. */

typedef enum gk_rt_flags {
    GK_RT_ALPHA = 1 << 0,

    GK_RT_DEPTH   = 1 << 1,
    GK_RT_STENCIL = 1 << 2,
} gk_rt_flags;

/* This creates a simple, 1-color-attachment FBO.  More may be added
   later.  There is nothing requiring the use of this command to later
   use gk_cmd_rt_bind/unbind, i.e., you can set up your own
   framebuffer. */
typedef struct gk_cmd_rt_create {
    gk_cmd parent;

    uint32_t width;
    uint32_t height;

    uint32_t rt_flags;       /* gk_rt_flags */
    uint32_t tex_flags;      /* gk_tex_flags */
    uint32_t tex_min_filter; /* gk_tex_filter */
    uint32_t tex_mag_filter; /* gk_tex_filter */

    /* These are set (based on flags) */
    unsigned int framebuffer;
    unsigned int dsbuffer;
    gk_texture   tex;
} gk_cmd_rt_create;

typedef struct gk_cmd_rt_destroy {
    gk_cmd parent;

    /* Anything nonzero will be deleted. */
    unsigned int framebuffer;
    unsigned int dsbuffer;
    gk_texture   tex;
} gk_cmd_rt_destroy;

typedef struct gk_cmd_rt_bind {
    gk_cmd       parent;
    unsigned int framebuffer;
} gk_cmd_rt_bind;

typedef struct gk_cmd_rt_unbind {
    gk_cmd parent;
} gk_cmd_rt_unbind;

/* Shaders **********************************************************/

typedef enum gk_shader_type {
    GK_SHADER_VERTEX,
    GK_SHADER_TESS_CONTROL,
    GK_SHADER_TESS_EVALUATION,
    GK_SHADER_GEOMETRY,
    GK_SHADER_FRAGMENT,
    GK_SHADER_COMPUTE,

    GK_SHADER_MAX
} gk_shader_type;

typedef struct gk_shader_source {
    gk_shader_type type;
    const char*    source;
} gk_shader_source;

typedef struct gk_program_source {
    size_t             nsources;
    gk_shader_source** source;

    /* Output */
    gk_program program;
} gk_program_source;

typedef struct gk_cmd_program_create {
    gk_cmd parent;

    size_t              nsources;
    gk_program_source** source;
} gk_cmd_program_create;

typedef struct gk_cmd_program_destroy {
    gk_cmd parent;

    size_t      nprograms;
    gk_program* program;
} gk_cmd_shader_destroy;

typedef struct gk_cmd_uniform_query {
    gk_cmd parent;

    /* Pass a pointer to the program */
    gk_program program;

    /* Pass an array of `nuniforms` strings to query */
    size_t       nuniforms;
    const char** names;

    /* Output; provide an array of gk_uniform with `nuniforms`
       elements */
    gk_uniform* uniforms;
} gk_cmd_uniform_query;

typedef enum gk_uniform_value_type {
    GK_UNIFORM_VALUE_FLOAT,
    GK_UNIFORM_VALUE_INT,
    GK_UNIFORM_VALUE_UINT,

    GK_UNIFORM_VALUE_VEC2,
    GK_UNIFORM_VALUE_VEC3,
    GK_UNIFORM_VALUE_VEC4,
    GK_UNIFORM_VALUE_MAT4,

    GK_UNIFORM_VALUE_MAX
} gk_uniform_value_type;

typedef struct gk_uniform_value {
    gk_uniform            location;
    gk_uniform_value_type type;
    size_t                count;

    union {
        /* If count > 1 or it's not scalar, use a pointer. */
        void* data;

        /* If count == 1 and it's a scalar type, use immediate fields. */
        int          i;
        unsigned int ui;
        float        f;
    } value;
} gk_uniform_value;

typedef struct gk_uniform_set {
    size_t            nuniforms;
    gk_uniform_value* values;
} gk_uniform_set;

/* A gk_program_data_set is a collection of state necessary for a
   particular draw operation, even---especially---if the operation
   (e.g., a quad command) buffers.

   Program data sets may share programs and data.  They are merely
   configurations of state.
*/
struct gk_program_data_set {
    gk_program      program;
    gk_uniform_set* uniforms;
};

#endif /* __GAMEKERNEL_GL_H__ */
