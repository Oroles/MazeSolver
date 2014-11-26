#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "kernel.h"
#include "ecrobot_interface.h"
#include "utils/shared_variables.h"
#include "utils/utils.h"
#include "actions/commands.h"
#include "localization.h"

// X functions
double __rx=MAP_RES/2;
int __x=0;
	int get_x() {
		return __x;
	}
	double get_realX() {
		return __rx;
	}
	void update_x(double dx) {
		__rx+=dx;
		__x= __rx>=0 ? __rx/MAP_RES : __rx/MAP_RES-1;
	}

// Y functions
double __ry=MAP_RES/2;
int __y=0;
	int get_y() {
		return __y;
	}
	double get_realY() {
		return __ry;
	}
	void update_y(double dy) {
		__ry+=dy;
		__y= __ry>=0 ? __ry/MAP_RES : __ry/MAP_RES-1;
	}

// W functions (angle)
double __w=0;
int __cp=EAST;

	int find_cardinal(double w) {
		if(w<CARD_PRECISION) return EAST;
		if(w<90-CARD_PRECISION) return NO_EA;
		if(w<=90+CARD_PRECISION) return NORTH;
		if(w<180-CARD_PRECISION) return NO_WE;
		if(w<=180+CARD_PRECISION) return WEST;
		if(w<270-CARD_PRECISION) return SO_WE;
		if(w<=270+CARD_PRECISION) return SOUTH;
		if(w<360-CARD_PRECISION) return SO_EA;
		return EAST;
	}

	double get_w() {
		return __w;
	}
	int get_cardinal_point() {
		return __cp;
	}
	void update_w(double w) {
		__w=w;
		if(__w>=360) __w-=360;
		else if(__w<0) __w+=360;
		__cp=find_cardinal(__w);
	}

// General functions
int is_inside_square(double rx, double ry, int side) {
	GetResource(SyncLocalization);
	double x_in_cell=get_realX()-get_x()*MAP_RES;
	double y_in_cell=get_realY()-get_y()*MAP_RES;
	ReleaseResource(SyncLocalization);
	if(x_in_cell<0) x_in_cell+=MAP_RES;
	if(y_in_cell<0) y_in_cell+=MAP_RES;

	if( x_in_cell<(MAP_RES/2+side) &&
		x_in_cell>(MAP_RES/2-side) &&
		y_in_cell<(MAP_RES/2+side) &&
		y_in_cell>(MAP_RES/2-side))
	{
		return TRUE;
	}else{
		return FALSE;
	}
}

int direction_of_next_cell( int current_x, int current_y, int next_x, int next_y )
{
	if ( current_x != next_x ) {
		if ( current_x < next_x ) return EAST;
		if ( current_x > next_x ) return WEST;
	}
	if ( current_y != next_y ) {
		if ( current_y < next_y ) return NORTH;
		if ( current_y > next_y ) return SOUTH;
	}
	return NO_CARD;
}

int dist_from_cell_cp(int cp) {
	GetResource(SyncLocalization);
	double x_in_cell=get_realX()-get_x()*MAP_RES;
	double y_in_cell=get_realY()-get_y()*MAP_RES;
	ReleaseResource(SyncLocalization);
	if(x_in_cell<0) x_in_cell+=MAP_RES;
	if(y_in_cell<0) y_in_cell+=MAP_RES;
	switch(cp) {
		case NORTH: return MAP_RES-y_in_cell;
		case EAST: return MAP_RES-x_in_cell;
		case SOUTH: return y_in_cell;
		case WEST: return x_in_cell;
		default: return -1;
	}
}

// Task functions
void init_localization() {
	int distF=get_distanceF();
	int distR=get_distanceR();
	int distL=get_distanceL();
	if(distF!=255+CENTER_TO_FRONT)
		__rx= MAP_RES-((distF*10) % MAP_RES);

	if(distL<distR)
		__ry= MAP_RES-((distL*10) % MAP_RES);
	else if(distR!=255+CENTER_TO_SIDES)
		__ry= (distR*10) % MAP_RES;
	
	if ( !is_inside_square(get_realX(),get_realY(),CENTER_RES) ) {
		double angle=angle_to_reach(MAP_RES/2,MAP_RES/2,__rx,__ry);

		turn_to_w(angle,20);
		WaitEvent(EndOfMovement);
		ClearEvent(EndOfMovement);
		go_forward(-20);
	}
}

void update_localization() {
	static int __last_wL=0;
	static int __last_wR=0;
	static double __w=0;
	static U8 event_sent=FALSE;

	if(is_inside_square(__rx,__ry,CENTER_RES)) {
		if(!event_sent) {
			SetEvent(MainController, CellCenter);
			event_sent=TRUE;
		}
	}
	else {
		event_sent=FALSE;
	}

	// Don't stop me, I need synchronized parameter values
	GetResource(RES_SCHEDULER);
	int temp_wL=get_wPositionL();
	int temp_wR=get_wPositionR();
	ReleaseResource(RES_SCHEDULER);
	
	// Initialization
	double wL,wR;
	wL=temp_wL-__last_wL;
	wR=temp_wR-__last_wR;
	
	if(abs(wL)>=0 || abs(wR)>=0) {
		__last_wL=temp_wL;
		__last_wR=temp_wR;
		// Computations
		wL=wL*L_CALIBRATION*CONV;
		wR=wR*R_CALIBRATION*CONV;
		double Vs=wL+wR;
		Vs=Vs/2;
		double w=wR-wL;
		w=w/W_DIST;
		__w+=w;

		double x,y;
		x=cos(__w);
		y=sin(__w);
		x=Vs*x;
		y=Vs*y;

		// Update Shared Variables
		GetResource(SyncLocalization);
		update_x(x);
		update_y(y);
		update_w(__w/RAD);
		ReleaseResource(SyncLocalization);
	}
}
