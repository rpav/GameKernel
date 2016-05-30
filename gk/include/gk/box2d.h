/******************************************************************
 * Box2D
 ******************************************************************/

#ifndef __GAMEKERNEL_BOX2D_H__
#define __GAMEKERNEL_BOX2D_H__

typedef struct gk_b2_world {
    float timestep;
    int velocity_iterations;
    int position_iterations;

    /* Internal data */
    void *data;
    void *draw;
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
    bool allow_sleep;

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

    /* Fixtures are created by specifying paths. Supported:

         GK_PATH_RECT:     Make a b2PolygonShape rectangle.  Note you still
                           specify x,y,w,h; not center and half-w/h.
  
         GK_PATH_CIRCLE:   Make a b2CircleShape.
  
         GK_PATH_MOVE_TO,
         GK_PATH_LINE_TO:  Only use move-to for the first vertex.  Still subject
                           to Box2D polygon vertex limits.  Must be convex.
  
         GK_PATH_BEGIN:    Reset values and start a new path/shape.
         GK_PATH_FILL:     End and create shape.
  
         GK_PATH_STROKE:   Use with move-to/line-to.  Create a b2ChainShape.
         GK_PATH_CLOSE:    Close a b2ChainShape.  Use before stroke.

         GK_PATH_DENSITY,
         GK_PATH_ELASTICITY,
         GK_PATH_FRICTION: Physics values.

         GK_PATH_CATEGORY,
         GK_PATH_CATEGORY_MASK,
         GK_PATH_GROUP:    Category/group values.
  
       Other path commands are not supported.  Also these are more limited than
       with NVG; e.g. you cannot add multiple shapes to a single path.

       You _can_ create multiple fixtures with one pathdef.  This is a
       large part of the point.
    */
    size_t pathlen;
    float *pathdef;
} gk_cmd_b2_fixture_create;

typedef struct gk_cmd_b2_step {
    gk_cmd parent;
    gk_b2_world *world;
} gk_cmd_b2_step;


typedef struct gk_cmd_b2_draw_debug {
    gk_cmd parent;

    gk_b2_world *world;
    int width;
    int height;
} gk_cmd_b2_draw_debug;

#endif  /* __GAMEKERNEL_BOX2D_H__ */