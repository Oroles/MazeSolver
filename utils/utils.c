#include <stdlib.h>
#include <math.h>
#include "ecrobot_interface.h"
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
		case NORTH: (*y)++; return TRUE;
		case EAST: (*x)++; return TRUE;
		case SOUTH: (*y)--; return TRUE;
		case WEST: (*x)--; return TRUE;
		default: return FALSE;
	}
}

void display_cp(int cp) {
	switch(cp) {
		case NORTH: display_string("NORTH"); break;
		case EAST: display_string("EAST"); break;
		case SOUTH: display_string("EAST"); break;
		case WEST: display_string("WEST"); break;
		default: display_string("????"); break;
	}
}

double angle_to_reach(double xi, double yi, double xf, double yf) {
	double opp=yf-yi,adj=xf-xi;
	double res;
	if(adj!=0 && opp!=0) {
		res=fabs(opp/adj);
		res=atan(res)/RAD;
	}
	else if(opp!=0) res=90;
	else res=0;

	if(opp>=0) {
		if(adj>=0) return res;
		else return 180-res;
	}
	else {
		if(adj>=0) return 360-res;
		else return 180+res;
	}
}

void cell_center(int x, int y, double *realx, double *realy) {
	*realx=(x+0.5)*MAP_RES;
	*realy=(y+0.5)*MAP_RES;
}
