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
		__x=__rx/MAP_RES;
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
		__y=__ry/MAP_RES;
	}

// W functions (angle)
double __w=0;
int __cp=0;

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

// Task functions
void init_localization() {
	int distF=get_distanceF();
	int distR=get_distanceR();
	int distL=get_distanceL();
	if(distF!=255)
		__rx= MAP_RES-((distF*10) % MAP_RES);

	if(distL<distR)
		__ry= MAP_RES-((distL*10) % MAP_RES);
	else if(distR!=255)
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

	if(is_inside_square(__rx,__ry,CENTER_RES)) {
		SetEvent(MainController, CellCenter);
	}
	if(is_inside_square(__rx,__ry,MAPPING_RES)) {
		SetEvent(Mapping, StartMapping);
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
		GetResource(UpdateLocker);
		update_x(x);
		update_y(y);
		update_w(__w/RAD);
		ReleaseResource(UpdateLocker);
	}
}
