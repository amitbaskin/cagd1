#pragma once



void init_menus();

void menu_callbacks( int id, int unUsed, PVOID userData );

void left_mouse_click_cb( int x, int y, PVOID userData );

void handle_num_samples_menu();
void handle_anim_speed_menu();

void toggle_check_menu( HMENU main_menu, UINT sub_menu_id );
int is_menu_checked( HMENU main_menu, UINT sub_menu_id );