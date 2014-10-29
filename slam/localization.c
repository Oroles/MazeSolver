#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "kernel.h"
#include "ecrobot_interface.h"
#include "utils/shared_variables.h"
#include "utils/utils.h"
#include "actions/movement.h"
#include "localization.h"

#define GET_REAL 	0
#define GET_FORMAL	1
#define UPDATE 	2


double access_x(double dx, int setMode) {
	static double __rx=0;
	static int __x=0;
	switch(setMode) {
		case UPDATE:
		__rx+=dx;
		__x=lround(__rx/MAP_RES);
		return 0;
		case GET_REAL:
		return __rx;
		case GET_FORMAL:
		return __x;
		default:
		return 0;
	}
}
	int get_x() {
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
		return (int) access_x(0,GET_FORMAL);
	}
	double get_realX() {
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
		return access_x(0,GET_REAL);
	}
	void update_x(double dx) {
		access_x(dx,UPDATE);
	}

double access_y(double dy, int setMode) {
	static double __ry=0;
	static int __y=0;
	switch(setMode) {
		case UPDATE:
		__ry+=dy;
		__y=lround(__ry/MAP_RES);
		return 0;
		case GET_REAL:
		return __ry;
		case GET_FORMAL:
		return __y;
		default:
		return 0;
	}
}
	int get_y() {
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
		return (int) access_y(0,GET_FORMAL);
	}
	double get_realY() {
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
		return access_y(0,GET_REAL);
	}
	void update_y(double dy) {
		access_y(dy,UPDATE);
	}

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

double access_w(double w, int setMode) {
	static double __w=0;
	static int __cp=0;
	switch(setMode) {
		case UPDATE:
		__w=w;
		if(__w>360) __w-=360;
		if(__w<0) __w+=360;
		__cp=find_cardinal(__w);
		return 0;
		case GET_REAL:
		return __w;
		case GET_FORMAL:
		return __cp;
		default:
		return 0;
	}
}
	double get_w() {
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
		return access_w(0,GET_REAL);
	}
	int get_cardinal_point() {
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
		return (int) access_w(0,GET_FORMAL);
	}
	void update_w(double w) {
		access_w(w,UPDATE);
	}

void update_localization() {
	static int __last_wL=0;
	static int __last_wR=0;
	static double __w=0;

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
