#include "utils.h"
#include "ecrobot_interface.h"

int next_cp(int cp) {
	switch(cp) {
		case NORTH: case NO_EA: return EAST;
		case EAST: case SO_EA: return SOUTH;
		case SOUTH: case SO_WE: return WEST;
		case WEST: case NO_WE: return NORTH;
		default: return -1;
	}
}

int previous_cp(int cp) {
	switch(cp) {
		case NORTH: case NO_EA: return WEST;
		case EAST: case SO_EA: return NORTH;
		case SOUTH: case SO_WE: return EAST;
		case WEST: case NO_WE: return SOUTH;
		default: return -1;
	}
}

int is_cp(int val) { return (val==NORTH)||(val==EAST)||(val==SOUTH)||(val==WEST); }

int coord_for_cp_square(int cp, int *x, int *y) {
	switch(cp) {
		case NORTH: *y++; return TRUE;
		case EAST: *x++; return TRUE;
		case SOUTH: *y--; return TRUE;
		case WEST: *x--; return TRUE;
		default: return FALSE;
	}
}

void display_cp(int cp) {
	display_clear(0);
	display_goto_xy(0,0);
	switch(cp) {
		case NORTH: display_string("NORTH"); break;
		case EAST: display_string("EAST"); break;
		case SOUTH: display_string("EAST"); break;
		case WEST: display_string("WEST"); break;
		default: display_string("????"); break;
	}
	display_update();
}
