#pragma once

void gk_process_spritesheet_create(gk_context *gk, gk_cmd_spritesheet_create *cmd);
void gk_process_spritesheet_destroy(gk_context *gk, gk_cmd_spritesheet_destroy *cmd);

void gk_load_ssf_texturepacker_json(gk_context *gk, gk_cmd_spritesheet_create *cmd,
                                    gk_spritesheet *sheet);
