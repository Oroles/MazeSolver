#include <stddef.h>
#include <math.h>
#include "kernel.h"
#include "ecrobot_interface.h"
#include "shared_variables.h"
#include "localization.h"

#define PI	3.14159265


int access_x(int x, int setMode) {
	static int __x=0;
	if(setMode) { __x=x; return 0; }
	else return __x;
}
	int get_x() {
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
		return access_x(0,0);
	}
	void set_x(int x) { access_x(x,1); }

int access_y(int y, int setMode) {
	static int __y=0;
	if(setMode) { __y=y; return 0; }
	else return __y;
} 
	int get_y() {
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
		return access_y(0,0);
	}
	void set_y(int y) { access_y(y,1); }

int access_d(int d, int setMode) {
	static int __d=0;
	if(setMode) { __d=d; return 0; }
	else return __d;
}
	int get_d() {
		GetResource(UpdateLocker);
		ReleaseResource(UpdateLocker);
		return access_d(0,0);
	}
	void set_d(int d) { access_d(d,1); }

double round(double val) {
	return floor(val + 0.5);
}

void update_localization() {
	static int __last_wL;
	static int __last_wR;

	// Don't stop me, I need synchronized parameter values
	GetResource(RES_SCHEDULER);
	int wL=get_wPositionL;
	int wR=get_wPositionR;
	ReleaseResource(RES_SCHEDULER);
	
	// Initialization
	int temp,temp2,temp3;
	temp=wL;
	wL -= __last_wL;
	__last_wL=temp;
	temp=wR;
	wR -= __last_wR;
	__last_wR=temp;
	// Computations
	wL=wL*R;
	wR=wR*R;
	double Vs=wL+wR;
	Vs=Vs/2;
	double w=wR-wL;
	w=w/D;
	double rad=PI/180.0;
	w=w*rad;
	double x=cos(w);
	double y=sin(w);
	x=Vs*x;
	y=Vs*y;
	x=round(x);
	y=round(y);
	temp= get_x();
	temp2=get_y();
	temp3=get_d();

	// Update Shared Variables
	GetResource(UpdateLocker);
	set_x(temp  + (int)x);
	set_y(temp2 + (int)y);
	set_d(temp3 + w);
	ReleaseResource(UpdateLocker);
}
