#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "kernel.h"
#include "ecrobot_interface.h"
#include "utils/shared_variables.h"
#include "utils/utils.h"
#include "actions/movement.h"
#include "localization.h"

#define CENTER_RES 20

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
		__x=lround(__rx/MAP_RES);
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
		__y=lround(__ry/MAP_RES);
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
int is_cell_center(double rx, double ry) {
	double x_in_cell=rx-__x*MAP_RES;
	double y_in_cell=ry-__y*MAP_RES;
	if( x_in_cell<(MAP_RES/2+CENTER_RES) &&
		x_in_cell>(MAP_RES/2-CENTER_RES) &&
		y_in_cell<(MAP_RES/2+CENTER_RES) &&
		y_in_cell>(MAP_RES/2-CENTER_RES))
		return TRUE;
	else
		return FALSE;
}

// Task functions
void init_localization() {
	__rx= MAP_RES-(get_distanceF() % MAP_RES);
	__ry= get_distanceR() % MAP_RES;
	if(__rx<MAP_RES/2) go_forward(-20);
	else if(__rx>MAP_RES/2) go_forward(20);
}

void update_localization() {
	static int __last_wL=0;
	static int __last_wR=0;
	static double __w=0;

	if(is_cell_center(get_realX(),get_realY())) {
		SetEvent(MainController, CellCenter);
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
		wL=wL*CONV;
		wR=wR*CONV;
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
