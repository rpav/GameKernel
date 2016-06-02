/******************************************************************
 * GL ... raw GL-related stuff including quad and sprite batching.
 * Well, entirely that at the moment.  But this is where Render
 * Texture, 3D, custom shaders, etc will end up.
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
    /* These do _not_ numerically correspond to GLenum */
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

/* Spritesheet support.  Note this is not specific to any format, nor
   are you required to use a loader. */
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

    /* Array of (nsprites) strings naming each correspondingly-indexed
       sprite.  This is filled in by loaders, but otherwise not used
       internally. */
    char **names;
} gk_spritesheet;

typedef struct gk_cmd_quadsprite {
    gk_cmd parent;

    gk_mat4 tfm;
    gk_spritesheet *sheet;
    unsigned int index;
} gk_cmd_quadsprite;


/* Render Texture.  While this is a "GL" thing, you may set up and
   start/end RT anywhere (e.g., in a CONFIG list).  This can wrap an
   NVG list to draw to a texture for instance.  Note that NVG requires
   a stencil buffer. */

typedef enum gk_rt_flags {
    GK_RT_ALPHA = 1 << 0,

    GK_RT_DEPTH = 1 << 1,
    GK_RT_STENCIL = 1 << 2,
} gk_rt_flags;

/* Note this creates a simple, 1-color-attachment FBO.  More may be
   added later.  There is nothing requiring the use of this command
   to later use gk_cmd_rt_bind/unbind. */
typedef struct gk_cmd_rt_create {
    gk_cmd parent;

    uint32_t width;
    uint32_t height;

    uint32_t rt_flags;          /* gk_rt_flags */
    uint32_t tex_flags;         /* gk_tex_flags */
    uint32_t tex_min_filter;    /* gk_tex_filter */
    uint32_t tex_mag_filter;    /* gk_tex_filter */

    /* These are set (based on flags) */
    unsigned int framebuffer;
    unsigned int dsbuffer;
    unsigned int tex;
} gk_cmd_rt_create;

typedef struct gk_cmd_rt_destroy {
    gk_cmd parent;

    /* Anything nonzero will be deleted. */
    unsigned int framebuffer;
    unsigned int dsbuffer;
    unsigned int tex;
} gk_cmd_rt_destroy;

typedef struct gk_cmd_rt_bind {
    gk_cmd parent;
    unsigned int framebuffer;
    unsigned int dsbuffer;
    unsigned int tex;
} gk_cmd_rt_bind;

typedef struct gk_cmd_rt_unbind {
    gk_cmd parent;
} gk_cmd_rt_unbind;

#endif  /* __GAMEKERNEL_GL_H__ */
