#include "utils.h"
#include "shared_variables.h"

static U8 __color = NXT_COLOR_UNKNOWN;
static U8 __color_counter = 0;

	void set_color(U8 color) {
		if(__color == color)
			if(__color_counter<255) __color_counter++;
		else
			__color_counter=0;
		__color = color;
	}
	U8 get_color() {
		return __color;
	}
	U8 get_color_counter() {
		return __color_counter;
	}
	
S32 access_distanceL(S32 distanceL, int setMode) {
	static S32 __distanceL=255+CENTER_TO_SIDES;
	if(setMode) { __distanceL = distanceL+CENTER_TO_SIDES; return 0; }
	else return __distanceL;
}
	void set_distanceL(S32 distanceL) {
		access_distanceL(distanceL,1);
	}
	S32 get_distanceL() {
		return access_distanceL(0,0);
	}
	
S32 access_distanceR(S32 distanceR, int setMode) {
	static S32 __distanceR=255+CENTER_TO_SIDES;
	if(setMode) { __distanceR = distanceR+CENTER_TO_SIDES; return 0; }
	else return __distanceR;
}
	void set_distanceR(S32 distanceR) {
		access_distanceR(distanceR,1);
	}
	S32 get_distanceR() {
		return access_distanceR(0,0);
	}
	
S32 access_distanceF(S32 distanceF, int setMode) {
	static S32 __distanceF=255+CENTER_TO_FRONT;
	if(setMode) { __distanceF = distanceF+CENTER_TO_FRONT; return 0; }
	else return __distanceF;
}
	void set_distanceF(S32 distanceF) {
		access_distanceF(distanceF,1);
	}
	S32 get_distanceF() {
		return access_distanceF(0,0);
	}
	
int access_wPositionL(int wPositionL, int setMode) {
	static int __wPositionL=0;
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
	static int __wPositionR=0;
	if(setMode) { __wPositionR = wPositionR; return 0; }
	else return __wPositionR;
}
	void set_wPositionR(int wPositionR) {
		access_wPositionR(wPositionR,1);
	}
	int get_wPositionR() {
		return access_wPositionR(0,0);
	}

