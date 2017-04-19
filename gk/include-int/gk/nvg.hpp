#pragma once

void gk_process_nvg(gk_context *gk, gk_bundle *bundle, gk_list_nvg *list);
void gk_process_nvg_font_create(gk_context *gk, gk_cmd_font_create *cmd);
void gk_process_nvg_font_face(gk_context *gk, gk_cmd_font_face* cmd);
void gk_process_nvg_font_style(gk_context *gk, gk_cmd_font_style *cmd);
void gk_process_nvg_image_create(gk_context *gk, gk_cmd_image_create *cmd);
void gk_process_nvg_image_destroy(gk_context *gk, gk_cmd_image_destroy *cmd);
