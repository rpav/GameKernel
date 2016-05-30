/******************************************************************
 * GL ... raw GL-related stuff including quad and sprite batching.
 * Well, entirely that at the moment.  But this is where Render
 * Texture, 3D, custom shaders, etc will end up.
 ******************************************************************/

#ifndef __GAMEKERNEL_GL_H__
#define __GAMEKERNEL_GL_H__

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

#endif  /* __GAMEKERNEL_GL_H__ */
