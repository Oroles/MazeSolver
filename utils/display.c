#include "kernel.h"
#include "ecrobot_interface.h"
#include "slam/localization.h"
#include "slam/mapping.h"
#include "actions/pid_control.h"
#include "shared_variables.h"
#include "display.h"

#define DISPLAY_NONE 	0
#define DISPLAY_DATA 	1
#define DISPLAY_MAP 	2
#define DISPLAY_PID 	3

#define MODE_DISPLAY 	0

void displayData() {
	display_clear(0);
	//display_goto_xy(0,0);display_string("LD");display_goto_xy(2,0);display_int(get_distanceL(),3);
	//display_goto_xy(0,1);display_string("FD");display_goto_xy(2,1);display_int(get_distanceF(),3);
	//display_goto_xy(0,2);display_string("RD");display_goto_xy(2,2);display_int(get_distanceR(),3);
	//display_goto_xy(0,3);display_string("WL");display_goto_xy(2,3);display_int(get_wPositionL(),4);
	//display_goto_xy(0,4);display_string("WR");display_goto_xy(2,4);display_int(get_wPositionR(),4);
	display_goto_xy(0,0);display_string("X");display_goto_xy(2,0);display_int((int)get_realX(),3);
	display_goto_xy(0,1);display_string("Y");display_goto_xy(2,1);display_int((int)get_realY(),3);
	//U8 color = get_color();
	//int result = color == NXT_COLOR_RED ? 1 : 0;
	//display_goto_xy(0,7);display_string("CL");display_goto_xy(2,7);display_int(result,3);
	display_goto_xy(0,2);display_string("W");display_goto_xy(2,2);display_int((int)get_w(),3);
	
	display_update();
}

void displayPID() {
	static int x=0,y=0;
	display_goto_xy(x,y);
	display_int(get_PID_error(),2);display_string(",");
	x=(x+3)%15;
	if(x==0) y=(y+1)%7;
	display_update();
}

void displayMap() {
	display_map_debug();
}

void update_display() {
	switch(MODE_DISPLAY) {
		case DISPLAY_NONE: break;
		case DISPLAY_DATA: displayData(); break;
		case DISPLAY_MAP: displayMap(); break;
		case DISPLAY_PID: displayPID(); break;
		default: break;
	}
}
