#ifndef _MAP_H
#define _MAP_H

void display_map_debug();
U8 get_cell_data(int pos_x, int pos_y);
void init_mapping( U8 init_val );
void update_map();
int is_wall_in_direction(int orientation,int pos_x, int pos_y);
int is_visited_in_direction(int orientation, int pos_x, int pos_y);
void coord_to_table_index(int *x, int *y);

#endif
