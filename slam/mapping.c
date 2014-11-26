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
#define THRESHOLD_DISTANCE 30

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

int detect_wall(S32 distance, int cp) {
	if(distance<=MIN_DISTANCE)
		return TRUE;
	if(distance*10 <= dist_from_cell_cp(cp)+10)
		return TRUE;
	return FALSE;
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
	static int count_front_walls = 0;
	static int count_left_walls = 0;
	static int count_right_walls = 0;

	if (!get_startMapping()) {
		return;
	}

	// If not ready (not enough inside the cell), return
	if(!ready) {
		if(is_inside_square(get_realX(),get_realY(),MAPPING_RES)) ready=TRUE;
		return;
	}

	GetResource(SyncLocalization);
	int pos_x = get_x();
	int pos_y = get_y();
	ReleaseResource(SyncLocalization);
	// If out of the map, return
	if (is_out_of_map(pos_x,pos_y)) return;
	U8 data;

	// If just enter in a new cell
	if ( ( last_pos_y != pos_y ) || ( last_pos_x != pos_x ) ) {
		count_front_walls = 0;
		count_left_walls = 0;
		count_right_walls = 0;
		ready=FALSE;

		if(pos_x<__min_x) __min_x=pos_x;
		else if(pos_x>__max_x) __max_x=pos_x;
		if(pos_y<__min_y) __min_y=pos_y;
		else if(pos_y>__max_y) __max_y=pos_y;

		int direction = direction_of_next_cell(pos_x,pos_y,last_pos_x,last_pos_y);

		int real_x = pos_x;
		int real_y = pos_y;

		coord_to_table_index(&pos_x,&pos_y);
		data = _map[pos_x][pos_y];

		if(direction != NO_CARD) {
			set_wall_state(&data, direction, NO_WALL);
			_map[pos_x][pos_y]=data;

			coord_to_table_index(&last_pos_x,&last_pos_y);
			U8 oldData = _map[last_pos_x][last_pos_y];
			set_wall_state(&oldData, next_cp(next_cp(direction)), NO_WALL );
			_map[last_pos_x][last_pos_y] = oldData;
		}
		last_pos_x = real_x;
		last_pos_y = real_y;
		return;
	}
	else {
		coord_to_table_index(&pos_x,&pos_y);
		data = _map[pos_x][pos_y];
	}

	int cardinal_point = get_cardinal_point();
	// Do measurements only if in a good direction
	if(is_cp(cardinal_point)) {
		int left_wall = detect_wall(get_distanceL(), cardinal_point);
		int right_wall = detect_wall(get_distanceR(), next_cp(cardinal_point));
		int front_wall = detect_wall(get_distanceF(), previous_cp(cardinal_point));

		//Add plus one for is wall and substract one for no wall
		count_front_walls = front_wall ? count_front_walls + 1 : count_front_walls - 1;
		count_left_walls = left_wall ? count_left_walls + 1 : count_left_walls - 1;
		count_right_walls = right_wall ? count_right_walls + 1 : count_right_walls - 1;

		if(count_front_walls>0) set_wall_state(&data, cardinal_point, IS_WALL);
		else set_wall_state(&data, cardinal_point, NO_WALL);

		if(count_right_walls>0) set_wall_state(&data, next_cp(cardinal_point), IS_WALL);
		else set_wall_state(&data, next_cp(cardinal_point), NO_WALL);

		if(count_left_walls>0) set_wall_state(&data, previous_cp(cardinal_point), IS_WALL);
		else set_wall_state(&data, previous_cp(cardinal_point), NO_WALL);
	}

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
