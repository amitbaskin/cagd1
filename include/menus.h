#pragma once



void init_menus();

void menu_callbacks( int id, int unUsed, PVOID userData );

void left_mouse_click_cb( int x, int y, PVOID userData );

void handle_num_samples_menu();
void handle_anim_speed_menu();
void handle_evolute_check_menu();
void handle_offset_curve_check_menu();
void handle_offset_curve_value_menu();
void handle_minimum_domain_menu();
void handle_maximum_domain_menu();

void handle_x_parameter_menu();
void handle_y_parameter_menu();
void handle_z_parameter_menu();

void redraw_cb();

void toggle_check_menu( HMENU main_menu, UINT sub_menu_id );
int is_menu_checked( HMENU main_menu, UINT sub_menu_id );

int is_show_evolute_menu_checked();
int is_show_offset_curve_menu_checked();

int are_all_anim_menus_unchecked();