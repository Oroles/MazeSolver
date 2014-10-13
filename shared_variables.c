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
	
int access_wpositionL(int wpositionL, int setMode) {
	static int __wpositionL;
	if(setMode) { __wpositionL = wpositionL; return 0; }
	else return __wpositionL;
}
	void set_wpositionL(int wpositionL) {
		access_wpositionL(wpositionL,1);
	}
	int get_wpositionL() {
		return access_wpositionL(0,0);
	}
	
int access_wpositionR(int wpositionR, int setMode) {
	static int __wpositionR;
	if(setMode) { __wpositionR = wpositionR; return 0; }
	else return __wpositionR;
}
	void set_wpositionR(int wpositionR) {
		access_wpositionL(wpositionR,1);
	}
	int get_wpositionR() {
		return access_wpositionR(0,0);
	}

