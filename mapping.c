#include "kernel.h"
#include "ecrobot_interface.h"
#include "mapping.h"
#include "shared_variables.h"
#include "localization.h"
#include "utils.h"
#include "node.h"

/*
  7   6   5   4   3   2   1   0
| x | x | x | x | x | x | x | x |
  I   W   I   W   I   W   I   W

  The bits
  	7,6 - are for the N wall
  	5,4 - are for the S wall
  	3,2 - are for the E wall
  	1,0 - are for the W wall

  I - 1 if the coresponding W it's known value
  	- 0 if we don't know what's there and then W doesn't matter

  W - 1 represents wall
  	- 0 represents no-wall
*/

U8 _map[MAP_WIDTH][MAP_HEIGHT];
#define THRESHOLD_DISTANCE 50

#define N_INFO_MASK  0x80
#define N_WALL_MASK 0x40
#define S_INFO_MASK  0x20
#define S_WALL_MASK 0x10
#define E_INFO_MASK  0x08
#define E_WALL_MASK 0x04
#define W_INFO_MASK  0x02
#define W_WALL_MASK 0x01

#define D 			0x01

void init_mapping( U8 init_val ) {
	for( int i = 0; i < MAP_WIDTH; ++i ) {
		for( int j = 0; j < MAP_HEIGHT; ++j ) {
			_map[i][j] = init_val;
		}
	}
}

void coord_to_table_index(int *x, int *y) {
	if ( *x < 0 ) {
		*x += MAP_WIDTH;
	}
	if ( *y < 0 ) {
		*y += MAP_HEIGHT;
	}
}

int set_masks(int cp, U8 *known_mask, U8 *wall_mask) {
	switch(cp) {
		case NORTH:
		*known_mask=N_INFO_MASK;
		*wall_mask=N_WALL_MASK;
		break;
		case EAST:
		*known_mask=E_INFO_MASK;
		*wall_mask=E_WALL_MASK;
		break;
		case SOUTH:
		*known_mask=S_INFO_MASK;
		*wall_mask=S_WALL_MASK;
		break;
		case WEST:
		*known_mask=W_INFO_MASK;
		*wall_mask=W_WALL_MASK;
		break;
		default:
		return FALSE;
		break;
	}
	return TRUE;
}

int get_wall_state(U8 data, int cp) {
	U8 known_mask;
	U8 wall_mask;
	if(set_masks(cp, &known_mask, &wall_mask)) {
		if ( ( data & known_mask ) != 0 ) {
			if ( ( data & wall_mask ) != 0 ) {
				return IS_WALL;
			}
			else {
				return NO_WALL;
			}
		}
		else {
			return UNKNOWN;
		}
	}
	else {
		return ERROR;
	}
}

void set_wall_state(U8 *data, int cp, int state) {
	U8 known_mask;
	U8 wall_mask;
	if(!set_masks(cp, &known_mask, &wall_mask)) return;

	switch(state) {
		case IS_WALL:
		*data|=known_mask;
		*data|=wall_mask;
		break;
		case NO_WALL:
		*data|=known_mask;
		*data&=(0xff -wall_mask);
		break;
		case UNKNOWN:
		*data&=(0xff -known_mask);
		*data&=(0xff -wall_mask);
		break;
		default:
		break;
	}
}

int is_wall_in_direction(int orientation, int pos_x, int pos_y) {
	coord_to_table_index(&pos_x,&pos_y);
	return get_wall_state(_map[pos_x][pos_y],orientation);
}

int is_visited_in_direction(int orientation, int pos_x, int pos_y) {
	
	coord_for_cp_square(orientation, &pos_x, &pos_y);
	coord_to_table_index(&pos_x,&pos_y);

	U8 data = _map[pos_x][pos_y];
	U8 result = 0x00;
	result += (get_wall_state(data,NORTH) != UNKNOWN) ? 1 : 0;
	result += (get_wall_state(data,EAST) != UNKNOWN) ? 1 : 0;
	result += (get_wall_state(data,SOUTH) != UNKNOWN) ? 1 : 0;
	result += (get_wall_state(data,WEST) != UNKNOWN) ? 1 : 0;
	if ( result >= 3 ) {
		return TRUE;
	}
	return FALSE;
}

int detect_wall(S32 distance) {
	if (distance < THRESHOLD_DISTANCE) {
		return TRUE;
	}
	return FALSE;
}

void update_map() {
	static int last_pos_x = -1;
	static int last_pos_y = -1;

	int left_wall = detect_wall(get_distanceL());
	int right_wall = detect_wall(get_distanceR());
	int front_wall = detect_wall(get_distanceF());
	int cardinal_point = get_cardinal_point();

	int pos_x = get_x();
	int pos_y = get_y();
	if (pos_x < -MAP_WIDTH || pos_x > MAP_WIDTH) return;
	if (pos_y < -MAP_HEIGHT || pos_y > MAP_HEIGHT) return;
	coord_to_table_index(&pos_x,&pos_y);
	
	U8 data = _map[pos_x][pos_y];

	if(is_cp(cardinal_point)) {
		if(front_wall) set_wall_state(&data, cardinal_point, IS_WALL);
		else set_wall_state(&data, cardinal_point, NO_WALL);

		if(right_wall) set_wall_state(&data, next_cp(cardinal_point), IS_WALL);
		else set_wall_state(&data, next_cp(cardinal_point), NO_WALL);

		if(left_wall) set_wall_state(&data, previous_cp(cardinal_point), IS_WALL);
		else set_wall_state(&data, previous_cp(cardinal_point), NO_WALL);
	}

	if ( ( last_pos_y != pos_y ) || ( last_pos_x != pos_x ) ) {
			if(is_cp(cardinal_point)) set_wall_state(&data, next_cp(next_cp(cardinal_point)), IS_WALL);
			last_pos_x = pos_x;
			last_pos_y = pos_y;
			SetEvent(MainController, NewDiscovery);
	}
}

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
	if ( ( current->x == stop_x ) && ( current->y == stop_y ) ) {
		return TRUE;
	}
	return FALSE;
}

/* Use manhattan distance */
int heuristic_function(struct node* current, int stop_x, int stop_y ) {
	int dx = abs( current->x - stop_x );
	int dy = abs( current->y - stop_y );
	int result = 0;
	result = D * ( dx + dy );
	return result;
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
			}
			neighbor = remove_first_node( &neighbors );
		}
		current = remove_first_node( &open_list );
	}
	//print_map( current );

	free_list( &open_list );
	free_list( &close_list );
}


void display_map(int row, int column, U8 matrix[row][column]) {
	display_clear(0);
	for( int i = 0; i < row; ++i ) {
		for( int j = 0; j < column; ++j ) {
			display_goto_xy( i, j );
			U8 result = 0x00;
			result |= ( matrix[i][j] & W_WALL_MASK );
			result |= ( ( matrix[i][j] & E_WALL_MASK ) >> 1 );
			result |= ( ( matrix[i][j] & S_WALL_MASK ) >> 2 );
			result |= ( ( matrix[i][j] & N_WALL_MASK ) >> 3 );
			display_hex( result, 1 );
		}
	}
	display_update();
}

void display_map_debug() {
	display_map(MAP_WIDTH,MAP_HEIGHT,_map);
}
