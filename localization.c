#include <stddef.h>
#include <math.h>
#include "kernel.h"
#include "ecrobot_interface.h"
#include "shared_variables.h"
#include "utils_params.h"
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
	if(w<CARD_PRECISION) return EA;
	if(w<90-CARD_PRECISION) return NE;
	if(w<90+CARD_PRECISION) return NO;
	if(w<180-CARD_PRECISION) return NW;
	if(w<180+CARD_PRECISION) return WE;
	if(w<270-CARD_PRECISION) return SW;
	if(w<270+CARD_PRECISION) return SO;
	if(w<360-CARD_PRECISION) return SE;
	return EA;
}

double access_w(double dw, int setMode) {
	static double __w=0;
	static int __cp=0;
	switch(setMode) {
		case UPDATE:
		__w+=dw;
		if(__w>360) __w=__w-360;
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
	void update_w(double dw) {
		access_w(dw,UPDATE);
	}

void update_localization() {
	static int __last_wL=0;
	static int __last_wR=0;

	// Don't stop me, I need synchronized parameter values
	GetResource(RES_SCHEDULER);
	int wL=get_wPositionL();
	int wR=get_wPositionR();
	ReleaseResource(RES_SCHEDULER);
	
	// Initialization

	int temp;
	temp=wL;
	wL -= __last_wL;
	__last_wL=temp;
	temp=wR;
	wR -= __last_wR;
	__last_wR=temp;
	
	// Computations
	wL=wL*CONV;
	wR=wR*CONV;
	double Vs=wL+wR;
	Vs=Vs/2;
	double w=wR-wL;
	w=w/W_DIST;

	double x,y;
	x=cos(w);
	y=sin(w);
	x=Vs*x;
	y=Vs*y;

	// Update Shared Variables
	GetResource(UpdateLocker);
	update_x(x);
	update_y(y);
	update_w(w);
	ReleaseResource(UpdateLocker);
}
