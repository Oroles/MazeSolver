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

U8 _map[MAP_WIDTH][MAP_HEIGHT];
#define THRESHOLD_DISTANCE 10

#define N_INFO_MASK  0x80
#define N_WALL_MASK 0x40
#define S_INFO_MASK  0x20
#define S_WALL_MASK 0x10
#define E_INFO_MASK  0x08
#define E_WALL_MASK 0x04
#define W_INFO_MASK  0x02
#define W_WALL_MASK 0x01

int coord_to_table_index(int *x, int *y) {
	if ( *x < 0 ) {
		*x += MAP_WIDTH;
	}
	if ( *y < 0 ) {
		*y += MAP_HEIGHT;
	}
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

void init_mapping() {
	for( int i = 0; i < MAP_WIDTH; ++i ) {
		for( int j = 0; j < MAP_HEIGHT; ++j ) {
			_map[i][j] = 0;
		}
	}
}

int is_wall(U8 data, U8 known_mask, U8 wall_mask) {
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

int is_wall_in_direction(int orientation) {
	int pos_x = get_x();
	int pos_y = get_y();
	coord_to_table_index(&pos_x,&pos_y);

	U8 data = _map[pos_x][pos_y];
	switch( orientation ) {
		case NORTH: return is_wall( data, N_INFO_MASK, N_WALL_MASK );
		case SOUTH: return is_wall( data, S_INFO_MASK, S_WALL_MASK );
		case EAST: return is_wall( data, E_INFO_MASK, E_WALL_MASK );
		case WEST: return is_wall( data, W_INFO_MASK, W_WALL_MASK );
	}
	return ERROR;
}

boolean is_visited_in_direction(int orientation) {
	int pos_x = get_x();
	int pos_y = get_y();
	coord_for_cp_square(int cp, &pos_x, &pos_y);
	coord_to_table_index(&pos_x,&pos_y);

	U8 data = _map[pos_x][pos_y];
	U8 result = 0x00;
	result += ( ( data & N_INFO_MASK ) != UNKNOWN ) ? 1 : 0;
	result += ( ( data & S_INFO_MASK ) != UNKNOWN ) ? 1 : 0;
	result += ( ( data & E_INFO_MASK ) != UNKNOWN ) ? 1 : 0;
	result += ( ( data & W_INFO_MASK ) != UNKNOWN ) ? 1 : 0;
	if ( result >= 3 ) {
		return true;
	}
	return false;
}


U8 get_unknown_cardinal(U8 mask) {
	if ( ( mask & N_INFO_MASK ) == 0 ) return N_INFO_MASK;
	if ( ( mask & W_INFO_MASK ) == 0 ) return W_INFO_MASK;
	if ( ( mask & S_INFO_MASK ) == 0 ) return S_INFO_MASK;
	if ( ( mask & E_INFO_MASK ) == 0 ) return E_INFO_MASK;
	return N_INFO_MASK;
}

boolean detect_wall(S32 distance) {
	if (distance < THRESHOLD_DISTANCE) {
		return true;
	}
	return false;
}

void update_map() {
	static int last_pos_x = -1;
	static int last_pos_y = -1;

	S32 left_distance = get_distanceL();
	S32 right_distance = get_distanceR();
	S32 front_distance = get_distanceF();
	int cardinal_point = get_cardinal_point();
	int pos_x = get_x();
	int pos_y = get_y();
	U8 result = 0x00;

	if ( ( pos_x < -MAP_WIDTH ) || ( pos_x > MAP_WIDTH ) ) {
		return;
	}
	if ( ( pos_y < -MAP_HEIGHT ) || (pos_y > MAP_HEIGHT ) ) {
		return;
	}

	coord_to_table_index(&pos_x,&pos_y);

	if( cardinal_point == NORTH ) {
		boolean north_wall = detect_wall(front_distance);
		boolean est_wall = detect_wall(right_distance);
		boolean west_wall = detect_wall(left_distance);	
		result |= (N_INFO_MASK | (north_wall ? N_WALL_MASK : 0x00));
		result |= (E_INFO_MASK | (est_wall ? E_WALL_MASK : 0x00));
		result |= (W_INFO_MASK | (west_wall ? W_WALL_MASK : 0x00));
	}
	if ( cardinal_point == EAST ) {
		boolean est_wall = detect_wall(front_distance);
		boolean south_wall = detect_wall(right_distance);
		boolean north_wall = detect_wall(left_distance);
		result |= (N_INFO_MASK | (north_wall ? N_WALL_MASK : 0x00));
		result |= (E_INFO_MASK | (est_wall ? E_WALL_MASK : 0x00));
		result |= (S_INFO_MASK | (south_wall ? S_WALL_MASK : 0x00));
	}
	if ( cardinal_point == WEST ) {
		boolean west_wall = detect_wall(front_distance);
		boolean south_wall = detect_wall(left_distance);
		boolean north_wall = detect_wall(right_distance);
		result |= (N_INFO_MASK | (north_wall ? N_WALL_MASK : 0x00));
		result |= (W_INFO_MASK | (west_wall ? W_WALL_MASK : 0x00));
		result |= (S_INFO_MASK | (south_wall ? S_WALL_MASK : 0x00));
	}
	if ( cardinal_point == SOUTH ) {
		boolean south_wall = detect_wall(front_distance);
		boolean est_wall = detect_wall(left_distance);
		boolean west_wall = detect_wall(right_distance);
		result |= (S_INFO_MASK | (south_wall ? S_WALL_MASK : 0x00));
		result |= (E_INFO_MASK | (est_wall ? E_WALL_MASK : 0x00));
		result |= (W_INFO_MASK | (west_wall ? W_WALL_MASK : 0x00));
	}

	_map[pos_x][pos_y] = result;
	if ( ( last_pos_y != pos_y ) || ( last_pos_x != pos_x ) ) {
			U8 unknown_cardinal = get_unknown_cardinal(_map[pos_x][pos_y]);
			_map[pos_x][pos_y] |= ( _map[pos_x][pos_y] | unknown_cardinal );
			last_pos_x = pos_x;
			last_pos_y = pos_y;
			SetEvent(MainController, NewDiscovery);
	}
}
