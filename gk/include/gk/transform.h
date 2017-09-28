/******************************************************************
 * Transforms
 *
 * You probably have libraries for transforms.  However, including
 * these as commands is handy because 1) GLM is used internally, and
 * it's pretty fast, and 2) you can chain them:
 *
 * That means you can specify and entire scenegraph as a series of
 * chained transform commands: transform and render in 1 call to
 * gk_process.  And if you chain physics in...
 *
 * Also handy if your language lacks some of these.
 ******************************************************************/

#ifndef __GAMEKERNEL_TFM_H__
#define __GAMEKERNEL_TFM_H__

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

typedef struct gk_cmd_tf_m {
    gk_cmd_tf parent;

    /* out = prior * m */
    gk_mat4 *m;
} gk_cmd_tf_m;

#endif /* __GAMEKERNEL_TFM_H__ */
