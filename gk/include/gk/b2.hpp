#pragma once

void gk_process_box2d(gk_context *gk, gk_bundle *bundle, gk_list *list);

void gk_process_b2_world_create(gk_context *gk, gk_cmd_b2_world_create *cmd);
void gk_process_b2_world_destroy(gk_context *gk, gk_cmd_b2_world_destroy *cmd);
