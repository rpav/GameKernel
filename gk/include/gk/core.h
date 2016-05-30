#ifndef __GAMEKERNEL_CORE_H__
#define __GAMEKERNEL_CORE_H__

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

#endif /* __GAMEKERNEL_CORE_H__ */
