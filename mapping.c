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

void display_map(int row, int column, U8 matrix[row][column]) {
	display_clear(0);
	for( int i = 0; i < row; ++i ) {
		for( int j = 0; j < column; ++j ) {
			display_goto_xy( i, j );
			display_hex( matrix[i][j], 1 );
		}
	}
}

void init_map() {
	for( int i = 0; i < 15; ++i ) {
		for( int j = 0; j < 7; ++j ) {
			_map[i][j] = 0;
		}
	}
}

void update_map() {
	U8 color = get_color();
	S32 left_distance = get_distanceL();
	S32 right_distance = get_distanceR();
	S32 front_distance = get_distanceF();
	int pos_x = get_x();
	int pos_y = get_y();

	U8 result = 0x00;
	if( left_distance > 10 ) {
		result += NO_L_WALL;
	}
	if ( right_distance > 10 ) {
		result += NO_R_WALL;
	}
	if ( front_distance > 10 ) {
		result += NO_F_WALL;
	}
	if ( color != NXT_COLOR_RED ) {
		result += STOP_POSITION;
	}

	if( pos_x < 0 ) {
		pos_x += 15;
	}
	if( pos_y < 0 ) {
		pos_y += 7;
	}

	_map[pos_x][pos_y] = result;
}
