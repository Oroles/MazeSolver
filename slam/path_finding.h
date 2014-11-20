#ifndef _PATHF_H
#define _PATHF_H

struct node* find_shortest_path( int start_x, int start_y, int stop_x, int stop_y );
struct node* find_unvisited_cell( int start_x, int start_y );
void print_path(struct node* current);
#endif
