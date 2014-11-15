#include "kernel.h"
#include "ecrobot_interface.h"
#include "utils/utils.h"
#include "mapping.h"
#include "node.h"
#include "path_finding.h"


#define D 			0x01

struct node* find_neighbors(struct node* current ) {
	struct node* neighbors = NULL;
	int orientation = NORTH;
	int contor = 0;
	while( contor != 4 ) {
		if ( is_wall_in_direction(orientation,current->x,current->y) == NO_WALL )
		{
			struct node* neighbor = create_empty_node();
			int pos_x = current->x;
			int pos_y = current->y;
			coord_for_cp_square(orientation, &pos_x, &pos_y);
			init_nod_position( neighbor, pos_x, pos_y );
			add_node(&neighbors, neighbor );
		}
		orientation = next_cp( orientation );
		++contor;
	}
	return neighbors;
}

int move_cost(struct node* current, struct node* neighbor) {
	return 1;
}

int is_stop_position(struct node* current, int stop_x, int stop_y )
{
	int current_x = current->x < 0 ? current->x + MAP_WIDTH : current->x;
	int current_y = current->y < 0 ? current->y + MAP_HEIGHT : current->y;
	if ( ( current_x == stop_x ) && ( current_y == stop_y ) ) {
		return TRUE;
	}
	return FALSE;
}

/* Use manhattan distance */
int heuristic_function(struct node* current, int stop_x, int stop_y ) {
	int current_x = current->x < 0 ? current->x + MAP_WIDTH : current->x;
	int current_y = current->y < 0 ? current->y + MAP_HEIGHT : current->y;
	int dx = abs( current_x - stop_x );
	int dy = abs( current_y - stop_y );
	int result = 0;
	result = D * ( dx + dy );
	return result;
}

void print_path(struct node* current) {
	U8 path_map[MAP_WIDTH][MAP_HEIGHT];
	for( int i = 0; i < MAP_WIDTH; ++i ) {
		for( int j = 0; j < MAP_HEIGHT; ++j ) {
			path_map[i][j] = 0x00;
		}
	}

	while( current != NULL ) {
		int pos_x = current->x;
		int pos_y = current->y;
		coord_to_table_index(&pos_x,&pos_y);
		path_map[ pos_x ][ pos_y ] = 1;
		current = current->parent;
	}

	display_clear(0);
	for ( int i = 0; i < MAP_WIDTH; ++i ) {
		for ( int j = 0; j < MAP_HEIGHT; ++j ) {
			display_goto_xy(i,j);
			display_hex( path_map[i][j], 1);
		}
	}
	display_update();
}

struct node* find_parent_node( struct node* current, struct node* son ) {
	while( !equal( current->parent, son ) ) {
		current = current->parent;
	}
	return current;
}

struct node* find_unvisited_cell( int start_x, int start_y ) {
	struct node* open_list = create_empty_node();
	struct node* close_list = NULL;
	init_nod_position( open_list, start_x, start_y );

	while( count( open_list ) != 0 ) {
		struct node* current = remove_first_node( &open_list );
		add_node( &close_list, current );
		struct node* neighbors = find_neighbors( current );
		struct node* neighbor = remove_first_node( &neighbors );
		while( neighbor != NULL ) {
			if ( find_node( &close_list, neighbor ) || find_node( &open_list, neighbor ) ) {
				free(neighbor);
				neighbor = remove_first_node( &neighbors );
				continue;
			}
			int pos_x = neighbor->x;
			int pos_y = neighbor->y;
			coord_to_table_index( &pos_x, &pos_y );
			U8 data = get_cell_data(pos_x, pos_y);
			neighbor->parent = current;
			if ( data == 0x00 ) {
				struct node* next_node = find_parent_node( neighbor, close_list );
				remove_node( &close_list, next_node );
				free( neighbor );
				free_list( &open_list );
				free_list( &close_list );
				free_list( &neighbors );
				return next_node;
			}
			add_node( &open_list, neighbor );
			neighbor = remove_first_node( &neighbors );
		}
	}
	struct node* current = remove_first_node( &close_list );
	free_list( &close_list );
	return current;
}

int direction_of_next_cell( int current_x, int current_y )
{
	struct node* next_cell = find_unvisited_cell( current_x, current_y );
	int next_x = next_cell->x;
	int next_y = next_cell->y;
	free( next_cell );
	if ( current_x != next_x ) {
		if ( current_x < next_x ) return EAST;
		if ( current_x > next_x ) return WEST;
	}
	if ( current_y != next_y ) {
		if ( current_y < next_y ) return NORTH;
		if ( current_y > next_y ) return SOUTH;
	}
	return NO_CARD;
}

void find_shortest_path( int start_x, int start_y, int stop_x, int stop_y ) {
	struct node* open_list = create_empty_node();
	struct node* close_list = NULL;

	init_nod_position( open_list, start_x, start_y );

	struct node* current = remove_first_node( &open_list );
	current->g_cost = 0;
	while( ( current != NULL ) && ( is_stop_position( current, stop_x, stop_y ) == FALSE ) ) {

		add_node( &close_list, current );

		struct node* neighbors = find_neighbors( current );
		struct node* neighbor = remove_first_node( &neighbors );
		while ( neighbor != NULL ) {
			if ( find_node(&close_list, neighbor) == TRUE ) {
				free(neighbor);
				neighbor = remove_first_node( &neighbors );
				continue;
			}
			int cost = current->g_cost + move_cost(current,neighbor);

			if ( (cost < neighbor->g_cost) && ( find_node(&open_list, neighbor) == FALSE ) ) {
				neighbor->g_cost = cost;
				neighbor->f_cost = cost + heuristic_function( neighbor, stop_x, stop_y );
				add_node_priority(&open_list, neighbor);
				neighbor->parent = current;
			} else {
				free( neighbor );
			}

			neighbor = remove_first_node( &neighbors );
		}
		current = remove_first_node( &open_list );
	}
	print_path( current );

	free_list( &open_list );
	free_list( &close_list );
	if ( current != NULL ) {
		free( current );
	}
}
