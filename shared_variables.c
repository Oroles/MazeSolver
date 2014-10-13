#include "shared_variables.h"

U8 access_color(U8 color, int setMode) {
	static U8 __color = 0;
	if(setMode) { __color = color; return 0; }
	else return __color;
}
	void set_color(U8 color) {
		access_color(color,1);
	}
	U8 get_color() {
		return access_color(0,0);
	}
	
S32 access_distanceL(S32 distanceL, int setMode) {
	static S32 __distanceL;
	if(setMode) { __distanceL = distanceL; return 0; }
	else return __distanceL;
}
	void set_distanceL(S32 distanceL) {
		access_distanceL(distanceL,1);
	}
	S32 get_distanceL() {
		return access_distanceL(0,0);
	}
	
S32 access_distanceR(S32 distanceR, int setMode) {
	static S32 __distanceR;
	if(setMode) { __distanceR = distanceR; return 0; }
	else return __distanceR;
}
	void set_distanceR(S32 distanceR) {
		access_distanceR(distanceR,1);
	}
	S32 get_distanceR() {
		return access_distanceR(0,0);
	}
	
S32 access_distanceF(S32 distanceF, int setMode) {
	static S32 __distanceF;
	if(setMode) { __distanceF = distanceF; return 0; }
	else return __distanceF;
}
	void set_distanceF(S32 distanceF) {
		access_distanceF(distanceF,1);
	}
	S32 get_distanceF() {
		return access_distanceF(0,0);
	}
	
int access_wPositionL(int wPositionL, int setMode) {
	static int __wPositionL;
	if(setMode) { __wPositionL = wPositionL; return 0; }
	else return __wPositionL;
}
	void set_wPositionL(int wPositionL) {
		access_wPositionL(wPositionL,1);
	}
	int get_wPositionL() {
		return access_wPositionL(0,0);
	}
	
int access_wPositionR(int wPositionR, int setMode) {
	static int __wPositionR;
	if(setMode) { __wPositionR = wPositionR; return 0; }
	else return __wPositionR;
}
	void set_wPositionR(int wPositionR) {
		access_wPositionL(wPositionR,1);
	}
	int get_wPositionR() {
		return access_wPositionR(0,0);
	}

