#include "utils.h"
#include "shared_variables.h"

static S16 __rgb[3];

void set_rgb(S16* rgb) {
	__rgb[0] = rgb[0];
	__rgb[1] = rgb[1];
	__rgb[2] = rgb[2];
}

int is_stop_color() {
	if (!between(__rgb[0],460,20) ) return 0;
	if (!between(__rgb[1],380,20) ) return 0;
	if (!between(__rgb[2],380,20) ) return 0;
	return 1;
}
	
S32 access_distanceL(S32 distanceL, int setMode) {
	static S32 __distanceL=255;
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
	static S32 __distanceR=255;
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
	static S32 __distanceF=255;
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
