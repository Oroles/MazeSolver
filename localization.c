#include <stddef.h>
#include "kernel.h"
#include "ecrobot_interface.h"
#include "localization.h"

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


void update_localization() {
	GetResource(UpdateLocker);
	set_x(1);
	set_y(1);
	set_d(1);
	ReleaseResource(UpdateLocker);
}
