#include "utils.h"

int next_cp(int cp) {
	switch(cp) {
		case NORTH: case NO_EA: return EAST;
		case EAST: case SO_EA: return SOUTH;
		case SOUTH: case SO_WE: return WEST;
		case WEST: case NO_WE: return NORTH;
		default: return -1;
	}
}

int is_cp(int val) { return (val==NORTH)||(val==EAST)||(val==SOUTH)||(val==WEST); }

int coord_for_cp_square(int cp, int *x, int *y) {
	switch(cp) {
		case NORTH: *y++; return true;
		case EAST: *x++; return true;
		case SOUTH: *y--; return true;
		case WEST: *x--; return true;
		default: return false;
	}
}