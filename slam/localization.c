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
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
		return __x;
	}
	double get_realX() {
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
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
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
		return __y;
	}
	double get_realY() {
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
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
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
		return __w;
	}
	int get_cardinal_point() {
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
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
	double x_in_cell=rx-__x*MAP_RES;
	double y_in_cell=ry-__y*MAP_RES;
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

int dist_from_cell_cp(U8 cp) {
	double x_in_cell=get_realX()-get_x()*MAP_RES;
	double y_in_cell=get_realY()-get_y()*MAP_RES;
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

void fix_x_and_y(double *x_fix, double *y_fix, U8 cp, U8 fix_val) {
	switch(cp) {
		case NORTH: *y_fix=*y_fix+fix_val;
		break;
		case SOUTH: *y_fix=*y_fix-fix_val;
		break;
		case EAST: *x_fix=*x_fix+fix_val;
		break;
		case WEST: *x_fix=*x_fix-fix_val;
		break;
	}
}
void fix_localization(double *x_fix, double *y_fix, double *w_fix) {
	U8 cp=__cp;
	if(is_cp(cp) && !is_inside_square(get_realX(),get_realY(),CENTER_RES)) {
		int dist=get_distanceF();
		int error=dist_from_cell_cp(cp)/10-(dist+CENTER_TO_FRONT);
		if(error>1) {
			fix_x_and_y(x_fix,y_fix,cp,0);
		}
		dist=get_distanceL();
		error=dist_from_cell_cp(previous_cp(cp))/10-(dist+CENTER_TO_SIDES);
		if(error>1) {
			fix_x_and_y(x_fix,y_fix,previous_cp(cp),1);
			*w_fix=*w_fix+0.016;
		}
		dist=get_distanceR();
		error=dist_from_cell_cp(next_cp(cp))/10-(dist+CENTER_TO_SIDES);
		if(error>1) {
			fix_x_and_y(x_fix,y_fix,next_cp(cp),1);
			*w_fix=*w_fix-0.016;
		}
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

		fix_localization(&x,&y,&__w);

		// Update Shared Variables
		GetResource(UpdateLocker);
		update_x(x);
		update_y(y);
		update_w(__w/RAD);
		ReleaseResource(UpdateLocker);
	}
}
