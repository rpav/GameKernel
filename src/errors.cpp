#include "gk/gk.hpp"

const char *gk_error_strings[] = {
    "None",                     // GK_ERROR_NONE

    // Lists
    "Recursive list reference", // GK_ERROR_LIST_RECURSION
    "List type unknown",        // GK_ERROR_LIST_UNKNOWN
    "List corruption",          // GK_ERROR_LIST_CORRUPT

    // Passes
    "Pass list index out of range", // GK_ERROR_PASS_BADLIST

    // Commands
    "Unknown or invalid commands" // GK_ERROR_CMD_UNKNOWN

    // Sprites
    "Sprite sheet format unknown", // GK_ERROR_SSF_UNKNOWN

    // GK_ERROR_MAX
    "Max error.  This is not a real error, you should not get this message"
};
