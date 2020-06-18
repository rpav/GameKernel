#pragma once

void gk_process_nvg_font_create(gk_context* gk, gk_cmd_font_create* cmd);
void gk_process_nvg_font_face(gk_context* gk, gk_cmd_font_face* cmd);
void gk_process_nvg_font_style(gk_context* gk, gk_cmd_font_style* cmd);
void gk_process_nvg_image_create(gk_context* gk, gk_cmd_image_create* cmd);
void gk_process_nvg_image_destroy(gk_context* gk, gk_cmd_image_destroy* cmd);
void gk_process_nvg_path(gk_context* gk, gk_bundle* bundle, gk_list_nvg* list, gk_cmd_path* cmd);
void gk_process_nvg_text(gk_context* gk, gk_list_nvg* list, gk_cmd_text* cmd);

void ensure_nvg_inframe(gk_context* gk, int w, int h, float r);
void ensure_nvg_outframe(gk_context* gk);
