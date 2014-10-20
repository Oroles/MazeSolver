#include "kernel.h"
#include "ecrobot_interface.h"
#include "mapping.h"
#include "shared_variables.h"
#include "localization.h"

/*
  7   6   5   4   3   2   1   0
| x | x | x | x | x | x | x | x |
  O   W   O   W   O   W   O   W

  The bits
  	7,6 - are for the N wall
  	5,4 - are for the S wall
  	3,2 - are for the E wall
  	1,0 - are for the W wall

  O - 1 if the coresponding W it's known value
  	- 0 if we don't know what's there and then W doesn't matter

  W - 1 represents wall
  	- 0 represents no-wall
*/

U8 _map[15][7];
#define THRESHOLD_DISTANCE 10

#define N_ORI_MASK  0x80
#define N_WALL_MASK 0x40
#define S_ORI_MASK  0x20
#define S_WALL_MASK 0x10
#define E_ORI_MASK  0x08
#define E_WALL_MASK 0x04
#define W_ORI_MASK  0x02
#define W_WALL_MASK 0x01

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
	display_map(15,7,_map);
}

void init_mapping() {
	for( int i = 0; i < 15; ++i ) {
		for( int j = 0; j < 7; ++j ) {
			_map[i][j] = 0;
		}
	}
}

boolean is_wall(S32 distance) {
	if (distance < THRESHOLD_DISTANCE) {
		return true;
	}
	return false;
}


U8 get_unknown_cardinal(U8 mask) {
	if ( ( mask & N_ORI_MASK ) == 0 ) return N_ORI_MASK;
	if ( ( mask & W_ORI_MASK ) == 0 ) return W_ORI_MASK;
	if ( ( mask & S_ORI_MASK ) == 0 ) return S_ORI_MASK;
	if ( ( mask & E_ORI_MASK ) == 0 ) return E_ORI_MASK;
	return N_ORI_MASK;
}


void update_map() {
	static int last_pos_x = 0;
	static int last_pos_y = 0;

	S32 left_distance = get_distanceL();
	S32 right_distance = get_distanceR();
	S32 front_distance = get_distanceF();
	int cardinal_point = get_cardinal_point();
	int pos_x = get_x();
	int pos_y = get_y();
	U8 result = 0x00;

	if ( ( pos_x < -15 ) || ( pos_x > 15 ) ) {
		return;
	}

	if ( ( pos_y < -7 ) || (pos_y > 7 ) ) {
		return;
	}

	if ( pos_x < 0 ) {
		pos_x += 15;
	}

	if ( pos_y < 0 ) {
		pos_y += 7;
	}

	if( cardinal_point == NO ) {
		boolean north_wall = is_wall(front_distance);
		boolean est_wall = is_wall(right_distance);
		boolean west_wall = is_wall(left_distance);	
		result |= (N_ORI_MASK | (north_wall ? N_WALL_MASK : 0x00));
		result |= (E_ORI_MASK | (est_wall ? E_WALL_MASK : 0x00));
		result |= (W_ORI_MASK | (west_wall ? W_WALL_MASK : 0x00));
	}
	if ( cardinal_point == EA ) {
		boolean est_wall = is_wall(front_distance);
		boolean south_wall = is_wall(right_distance);
		boolean north_wall = is_wall(left_distance);
		result |= (N_ORI_MASK | (north_wall ? N_WALL_MASK : 0x00));
		result |= (E_ORI_MASK | (est_wall ? E_WALL_MASK : 0x00));
		result |= (S_ORI_MASK | (south_wall ? S_WALL_MASK : 0x00));
	}
	if ( cardinal_point == WE ) {
		boolean west_wall = is_wall(front_distance);
		boolean south_wall = is_wall(left_distance);
		boolean north_wall = is_wall(right_distance);
		result |= (N_ORI_MASK | (north_wall ? N_WALL_MASK : 0x00));
		result |= (W_ORI_MASK | (west_wall ? W_WALL_MASK : 0x00));
		result |= (S_ORI_MASK | (south_wall ? S_WALL_MASK : 0x00));
	}
	if ( cardinal_point == SO ) {
		boolean south_wall = is_wall(front_distance);
		boolean est_wall = is_wall(left_distance);
		boolean west_wall = is_wall(right_distance);
		result |= (S_ORI_MASK | (south_wall ? S_WALL_MASK : 0x00));
		result |= (E_ORI_MASK | (est_wall ? E_WALL_MASK : 0x00));
		result |= (W_ORI_MASK | (west_wall ? W_WALL_MASK : 0x00));
	}

	_map[pos_x][pos_y] = result;
	if ( ( last_pos_y != pos_y ) || ( last_pos_x != pos_x ) ) {
			U8 unknown_cardinal = get_unknown_cardinal(_map[pos_x][pos_y]);
			_map[pos_x][pos_y] |= ( _map[pos_x][pos_y] | unknown_cardinal );
			last_pos_x = pos_x;
			last_pos_y = pos_y;
	}
}
