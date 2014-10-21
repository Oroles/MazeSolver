#ifndef _MAP_H
#define _MAP_H

void display_map_debug();
void init_mapping();
void update_map();
int is_wall_in_direction(int orientation);
boolean is_visited_in_direction(int orientation);


#endif
