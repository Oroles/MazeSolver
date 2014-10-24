#ifndef _MAP_H
#define _MAP_H

void display_map_debug();
void init_mapping( U8 init_val );
void update_map();
int is_wall_in_direction(int orientation,int pos_x, int pos_y);
int is_visited_in_direction(int orientation, int pos_x, int pos_y);
void find_shortest_path( int start_x, int start_y, int stop_x, int stop_y );


#endif
