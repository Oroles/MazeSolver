#include "kernel.h"
#include "ecrobot_interface.h"
#include "mapping.h"
#include "shared_variables.h"
#include "localization.h"
#include "utils.h"

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

//Will be needed for shortest path algorithm
int max_min_x = 0; //the smallest x coord
int max_min_y = 0; //the smallest y coord

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

void init_mapping() {
	for( int i = 0; i < MAP_WIDTH; ++i ) {
		for( int j = 0; j < MAP_HEIGHT; ++j ) {
			_map[i][j] = 0;
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

int is_wall_in_direction(int orientation) {
	int pos_x = get_x();
	int pos_y = get_y();
	coord_to_table_index(&pos_x,&pos_y);

	return get_wall_state(_map[pos_x][pos_y],orientation);
}

int is_visited_in_direction(int orientation) {
	int pos_x = get_x();
	int pos_y = get_y();
	
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

	if ( pos_x < max_min_x ) max_min_x = pos_x;
	if ( pos_y < max_min_y ) max_min_y = pos_y;

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

void reverse_array( U8* data, int start, int stop ){
	for( start; start < stop; ++start, --stop ) {
		U8 aux = data[start];
		data[start] = data[stop];
		data[stop] = aux;
	}
}

void shift_array( U8* data, int array_size, int nr_shift ) {
	reverse_array( data, 0, array_size - 1);
	reverse_array( data, 0, nr_shift - 1);
	reverse_array( data, nr_shift, array_size - 1);
}

void shift_map_y( int nr_shift ) {
		for ( int i = 0; i < MAP_WIDTH; ++i ) {
			shift_array( _map[i], MAP_HEIGHT, nr_shift );
		}
}

void shift_map_x( int nr_shift ) {
	for ( int count = 0; count < nr_shift; ++count ) {
		for( int j = 0; j < MAP_HEIGHT; ++j ) {
			U8 auxSet = _map[0][j];
			for ( int i = 1; i < MAP_WIDTH; ++i ) {
				U8 aux = _map[i][j];
				_map[i][j] = auxSet;
				auxSet = aux;
			}
			_map[0][j] = auxSet;
		}
	}
}

void find_shortest_path( int start_x, int start_y, int stop_x, int stop_y ) {

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
