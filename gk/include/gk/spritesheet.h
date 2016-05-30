/******************************************************************
 * Spritesheet loading
 *
 * Note this isn't necessary to use spritesheets: you can fill out and
 * manage your own gk_spritesheet structs, which may be pretty handy
 * if you generate them procedurally.
 ******************************************************************/

#ifndef __GAMEKERNEL_SPRITESHEET_H__
#define __GAMEKERNEL_SPRITESHEET_H__

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

#endif  /* __GAMEKERNEL_SPRITESHEET_H__ */
