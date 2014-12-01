#include "kernel.h"
#include "ecrobot_interface.h"
#include "mapping.h"
#include "utils/shared_variables.h"
#include "utils/utils.h"
#include "localization.h"

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
#define THRESHOLD_DISTANCE 15

#define N_INFO_MASK  0x80
#define N_WALL_MASK 0x40
#define S_INFO_MASK  0x20
#define S_WALL_MASK 0x10
#define E_INFO_MASK  0x08
#define E_WALL_MASK 0x04
#define W_INFO_MASK  0x02
#define W_WALL_MASK 0x01

int __min_x=0;
int __min_y=0;
int __max_x=0;
int __max_y=0;

U8 is_out_of_map(int pos_x, int pos_y) {
	if((__max_x-__min_x>=MAP_WIDTH-1 && (pos_x>__max_x || pos_x<__min_x)) ||
			(__max_y-__min_y>=MAP_HEIGHT-1 && (pos_y>__max_y || pos_y<__min_y)))
		return TRUE;
	return FALSE;
}

U8 get_cell_data(int pos_x, int pos_y) {
	return _map[pos_x][pos_y];
}

void coord_to_table_index(int *x, int *y) {
	if ( *x < 0 ) {
		*x += MAP_WIDTH;
	}
	if ( *y < 0 ) {
		*y += MAP_HEIGHT;
	}
}

U8 set_masks(int cp, U8 *known_mask, U8 *wall_mask) {
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

U8 get_wall_state(U8 data, int cp) {
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

U8 is_wall_in_direction(int orientation, int pos_x, int pos_y) {
	coord_to_table_index(&pos_x,&pos_y);
	return get_wall_state(_map[pos_x][pos_y],orientation);
}

U8 is_visited_in_direction(int orientation, int pos_x, int pos_y) {
	coord_for_cp_square(orientation, &pos_x, &pos_y);
	if(is_out_of_map(pos_x,pos_y)) return OUT_OF_MAP;

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

int detect_wall(S32 distance, U8 cp) {
	int wall_diff = dist_from_cell_cp(cp)/10 - distance; // distance between the side cell and the closest wall
	if(wall_diff<-2*THRESHOLD_DISTANCE) return -2*THRESHOLD_DISTANCE;
	return wall_diff+THRESHOLD_DISTANCE;
}

void init_mapping( U8 init_val ) {
	for( int i = 0; i < MAP_WIDTH; ++i ) {
		for( int j = 0; j < MAP_HEIGHT; ++j ) {
			_map[i][j] = init_val;
		}
	}
	// Init value for the back wall of the first cell
	set_wall_state(&_map[0][0], WEST, NO_WALL);
}

void update_map() {
	static U8 ready=FALSE;
	static int last_pos_x = 0;
	static int last_pos_y = 0;
	//Stores the average value of walls
	static int wall_counter[4] = {0};

	// CONDITIONS OF EXECUTION
	// COND1: If not ready (not enough inside the cell), RETURN
	if(!ready) {
		if(is_inside_square(MAPPING_RES)) ready=TRUE;
		return;
	}

	GetResource(SyncLocalization);
	int pos_x = get_x();
	int pos_y = get_y();
	ReleaseResource(SyncLocalization);

	// COND2: If out of the map, RETURN
	if (is_out_of_map(pos_x,pos_y)) return;

	// COND3: If just enter in a new cell, init and RETURN
	if ( ( last_pos_y != pos_y ) || ( last_pos_x != pos_x ) ) {
		for(int i=0 ; i<4 ; i++) wall_counter[i] =0;
		ready=FALSE;

		if(pos_x<__min_x) __min_x=pos_x;
		else if(pos_x>__max_x) __max_x=pos_x;
		if(pos_y<__min_y) __min_y=pos_y;
		else if(pos_y>__max_y) __max_y=pos_y;

		U8 direction = direction_of_next_cell(pos_x,pos_y,last_pos_x,last_pos_y);

		int temp_last_x = last_pos_x;
		int temp_last_y = last_pos_y;
		last_pos_x = pos_x;
		last_pos_y = pos_y;

		coord_to_table_index(&pos_x,&pos_y);

		if(direction != NO_CARD) {
			set_wall_state(&_map[pos_x][pos_y], direction, NO_WALL);

			coord_to_table_index(&temp_last_x,&temp_last_y);
			set_wall_state(&_map[temp_last_x][temp_last_y], next_cp(next_cp(direction)), NO_WALL );
		}
		return;
	}

	U8 front_cp = get_cardinal_point();
	// COND4: If not oriented in a good direction, RETURN
	if(!is_cp(front_cp)) return;

	// NORMAL BEHAVIOUR
	U8 left_cp=previous_cp(front_cp);
	U8 right_cp=next_cp(front_cp);

	coord_to_table_index(&pos_x,&pos_y);
	U8 data = _map[pos_x][pos_y];

	//Add a positive value when there is wall, negative otherwise
	wall_counter[left_cp] += detect_wall(get_distanceL()+CENTER_TO_SIDES, left_cp);
	wall_counter[right_cp] += detect_wall(get_distanceR()+CENTER_TO_SIDES, right_cp);
	wall_counter[front_cp] += detect_wall(get_distanceF()+CENTER_TO_FRONT, front_cp);

	if(wall_counter[left_cp]>0) set_wall_state(&data, left_cp, IS_WALL);
	else if(wall_counter[left_cp]<0) set_wall_state(&data, left_cp, NO_WALL);

	if(wall_counter[right_cp]>0) set_wall_state(&data, right_cp, IS_WALL);
	else if(wall_counter[right_cp]<0) set_wall_state(&data, right_cp, NO_WALL);

	if(wall_counter[front_cp]>0) set_wall_state(&data, front_cp, IS_WALL);
	else if(wall_counter[front_cp]<0) set_wall_state(&data, front_cp, NO_WALL);

	_map[pos_x][pos_y]=data;
}

void display_map(int width, int height, U8 matrix[width][height]) {
	int x=__min_x,y=__min_y;
	coord_to_table_index(&x,&y);
	display_clear(0);
	for( int i = 0; i < width; ++i ) {
		for( int j = 0; j < height; ++j ) {
			display_goto_xy(i,j);
			U8 result = 0x00;
			result |= ( matrix[x][y] & W_WALL_MASK );
			result |= ( ( matrix[x][y] & E_WALL_MASK ) >> 1 );
			result |= ( ( matrix[x][y] & S_WALL_MASK ) >> 2 );
			result |= ( ( matrix[x][y] & N_WALL_MASK ) >> 3 );
			display_hex( result, 1 );
			y=(y+1) % height;
		}
		x=(x+1) % width;
	}
	display_update();
}

void display_map_debug() {
	display_map(MAP_WIDTH,MAP_HEIGHT,_map);
}
