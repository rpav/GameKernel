#include <stdlib.h>

#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/spritesheet.hpp"
#include "gk/log.hpp"

void gk_process_spritesheet_create(gk_context *gk, gk_cmd_spritesheet_create *cmd) {
    auto sheet = (gk_spritesheet*)malloc(sizeof(gk_spritesheet));

    switch(cmd->format) {
        case GK_SSF_TEXTUREPACKER_JSON:
            gk_load_ssf_texturepacker_json(gk, cmd, sheet);
            break;

        default:
            LOG("Unknown sprite sheet format ", cmd->format);
            gk_seterror(gk, GK_ERROR_SSF_UNKNOWN);
            break;
    }
    // If there is an error, the loader should free everything it
    // allocates
    if(gk_haserror(gk)) goto error;

    cmd->sheet = sheet;
    return;

 error:
    free(sheet);
}

void gk_process_spritesheet_destroy(gk_context *gk, gk_cmd_spritesheet_destroy *cmd) {
    auto sheet = cmd->sheet;
    GL_CHECK(glDeleteTextures(1, (GLuint*)&sheet->tex));

 gl_error:
    free(sheet->sprites);

    for(int i = 0; i < sheet->nsprites; ++i)
        free(sheet->names[i]);

    free(sheet->names);
    free(sheet);
}
