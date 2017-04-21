/******************************************************************
 * Box2D
 ******************************************************************/

#ifndef __GAMEKERNEL_BOX2D_H__
#define __GAMEKERNEL_BOX2D_H__

typedef struct gk_b2_world_data gk_b2_world_data;

typedef struct gk_b2_world {
    float timestep;
    int velocity_iterations;
    int position_iterations;

    /* Internal data */
    gk_b2_world_data *data;
} gk_b2_world;

/* These should correspond to b2BodyType */
typedef enum gk_b2_body_type {
    GK_B2_BODY_TYPE_STATIC = 0,
    GK_B2_BODY_TYPE_KINEMATIC,
    GK_B2_BODY_TYPE_DYNAMIC
} gk_b2_body_type;

typedef struct gk_b2_body_data gk_b2_body_data;

typedef struct gk_b2_body {
    /* Set or zero these.  If set (e.g., point them to members in a
       gk_cmd_tf_trs), the internal Box2D body values will be copied
       there during a gk_cmd_b2_iter_bodies, if the body is not
       asleep. */
    gk_vec2 *position;
    float *angle;

    /* These aren't pointers because they take about the same space
       and you probably don't need the values elsewhere */
    gk_vec2 velocity;
    float angular_velocity;

    /* These will be assigned/updated during gk_cmd_b2_iter_bodies. */
    bool is_awake;

    /* Whatever you want */
    void *user_data;

    /* Internal */
    gk_b2_body_data *data;
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
    bool do_sleep;
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

typedef struct gk_cmd_b2_body_update {
    gk_cmd parent;

    gk_b2_body *body;
    gk_vec2 translate;
    float angle;
} gk_cmd_b2_body_update;

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

         GK_PATH_SENSOR:   Make the fixture a sensor (no parameters)

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

typedef enum gk_b2_fixture_update_flags {
    GK_B2_FIXTURE_UPDATE_DENSITY    = (1<<0),
    GK_B2_FIXTURE_UPDATE_ELASTICITY = (1<<1),
    GK_B2_FIXTURE_UPDATE_FRICTION   = (1<<2)
} gk_b2_fixture_update_flags;

typedef struct gk_cmd_b2_fixture_update {
    gk_cmd parent;

    /* This will update EVERY fixture with this ID */
    gk_b2_body *body;
    int id;

    /* gk_b2_fixture_update_flags */
    unsigned int update;

    float density;
    float elasticity;
    float friction;
} gk_cmd_b2_fixture_update;

typedef struct gk_b2_contact_pair {
    /* You get A and B; they may be in any order. */
    gk_b2_body *a;
    gk_b2_body *b;

    /* Fixture IDs, if created with IDs, or 0 */
    int id_a;
    int id_b;

    /* Contact > 0; Separation < 0 */
    int contact;

    /* Ignore the man behind the curtains */
#ifdef __cplusplus
    gk_b2_contact_pair() = default;
    gk_b2_contact_pair(gk_b2_body *a_, gk_b2_body *b_, int fixa, int fixb)
      : a(a_), b(b_), id_a(fixa), id_b(fixb) {
        contact = 0;
    }
#endif
} gk_b2_contact_pair;

typedef struct gk_cmd_b2_step {
    gk_cmd parent;
    gk_b2_world *world;

    /* This collision list is managed internally.  Do not
       free it.  The contents of this are *only* valid until
       you issue another gk_cmd_b2_step on the same world. */
    gk_b2_contact_pair **collisions;
    size_t ncollisions;
} gk_cmd_b2_step;

typedef struct gk_cmd_b2_iter_bodies {
    gk_cmd parent;
    gk_b2_world *world;
} gk_cmd_b2_iter_bodies;


/* Forces */
typedef struct gk_cmd_b2_force {
    gk_cmd parent;

    gk_b2_body *body;
    gk_vec2 force;
    gk_vec2 point;
    bool wake;
} gk_cmd_b2_force;

typedef struct gk_cmd_b2_torque {
    gk_cmd parent;

    gk_b2_body *body;
    float torque;
    bool wake;
} gk_cmd_b2_torque;

typedef struct gk_cmd_b2_linear_impulse {
    gk_cmd parent;

    gk_b2_body *body;
    gk_vec2 impulse;
    gk_vec2 point;
    bool wake;
} gk_cmd_b2_linear_impulse;

typedef struct gk_cmd_b2_angular_impulse {
    gk_cmd parent;

    gk_b2_body *body;
    float impulse;
    bool wake;
} gk_cmd_b2_angular_impulse;


typedef struct gk_cmd_b2_set_velocity {
    gk_cmd parent;

    gk_b2_body *body;
    gk_vec2 linear;
    float angular;
} gk_cmd_b2_set_velocity;

/* Debug Draw */
typedef struct gk_cmd_b2_draw_debug {
    gk_cmd parent;

    gk_b2_world *world;
    gk_vec2 resolution;
    gk_vec2 translate;
    gk_vec2 scale;
} gk_cmd_b2_draw_debug;


#endif  /* __GAMEKERNEL_BOX2D_H__ */
