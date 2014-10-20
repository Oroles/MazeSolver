#include "kernel.h"
#include "ecrobot_interface.h"
#include "mapping.h"
#include "shared_variables.h"
#include "localization.h"

U8 _map[15][7];
#define THRESHOLD_DISTANCE 10
#define NO_L_WALL 0x01
#define NO_R_WALL 0x02
#define NO_F_WALL 0x04
#define STOP_POSITION 0x08;

S32 left_distance_array[10];
S32 right_distance_array[10];
S32 front_distance_array[10];
U8 color_array[10];
int count_left_distance_array;
int count_right_distance_array;
int count_front_distance_array;
int count_color_array;

void display_map(int row, int column, U8 matrix[row][column]) {
	display_clear(0);
	for( int i = 0; i < row; ++i ) {
		for( int j = 0; j < column; ++j ) {
			display_goto_xy( i, j );
			display_hex( matrix[i][j], 1 );
		}
	}
	display_update();
}

void display_map_debug() {
	display_map(15,7,_map);
}

int corectPositionS32(S32* array, int size) {
	for( int i = 0; i < size; ++i ) {
		int counter = 0;
		for ( int j = 0; j < size; ++j ) {
			if ( array[i] == array[j] )
			{
				counter++;
			}
		}
		if ( counter >= ( size / 2 ) ) {
			return i;
		}
	}
	return 0;
}

int corectPositionU8(U8* array, int size) {
	for( int i = 0; i < size; ++i ) {
		int counter = 0;
		for ( int j = 0; j < size; ++j ) {
			if ( array[i] == array[j] )
			{
				counter++;
			}
		}
		if ( counter >= ( size / 2 ) ) {
			return i;
		}
	}
	return 0;
}

void init_mapping() {
	for( int i = 0; i < 15; ++i ) {
		for( int j = 0; j < 7; ++j ) {
			_map[i][j] = 0;
		}
	}

	count_left_distance_array = 0;
	count_right_distance_array = 0;
	count_front_distance_array = 0;
	count_color_array = 0;
}

void update_map() {
	static int last_pos_x = 0;
	static int last_pos_y = 0;


	U8 color = get_color();
	S32 left_distance = get_distanceL();
	S32 right_distance = get_distanceR();
	S32 front_distance = get_distanceF();
	int pos_x = get_x();
	int pos_y = get_y();

	if ( ( last_pos_y == pos_y ) && ( last_pos_x == pos_x ) ) {
		left_distance_array[count_left_distance_array++] = left_distance;
		right_distance_array[count_right_distance_array++] = right_distance;
		front_distance_array[count_front_distance_array++] = front_distance;
		color_array[count_color_array] = color;
	}
	else
	{
		U8 corect_color = color_array[ corectPositionU8( color_array, count_color_array ) ];
		S32 corect_left_distance = left_distance_array[ corectPositionS32( left_distance_array, count_left_distance_array ) ];
		S32 corect_right_distance = right_distance_array[ corectPositionS32( right_distance_array, count_right_distance_array ) ];
		S32 corect_front_distance = front_distance_array[ corectPositionS32( front_distance_array, count_front_distance_array ) ];

		U8 result = 0x00;
		if( corect_left_distance > 10 ) {
			result += NO_L_WALL;
		}
		if ( corect_right_distance > 10 ) {
			result += NO_R_WALL;
		}
		if ( corect_front_distance > 10 ) {
			result += NO_F_WALL;
		}
		if ( corect_color == NXT_COLOR_RED ) {
			result += STOP_POSITION;
		}

		if( pos_x < 0 ) {
			pos_x += 15;
		}
		if( pos_y < 0 ) {
			pos_y += 7;
		}

		_map[last_pos_x][last_pos_y] = result;

		last_pos_y = pos_y;
		last_pos_x = pos_x;
	}
}
