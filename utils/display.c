#include "kernel.h"
#include "ecrobot_interface.h"
#include "slam/localization.h"
#include "slam/mapping.h"
#include "actions/pid_control.h"
#include "actions/commands.h"
#include "shared_variables.h"
#include "utils.h"
#include "display.h"

#define DISPLAY_NONE 	0
#define DISPLAY_DATA 	1
#define DISPLAY_MAP 	2
#define DISPLAY_PID 	3
#define DISPLAY_SCHED 	4

#define MODE_DISPLAY 	0

void displayData() {
	display_clear(0);
	display_goto_xy(0,0);display_string("LD");display_goto_xy(2,0);display_int(get_distanceL(),3);
	display_goto_xy(0,1);display_string("FD");display_goto_xy(2,1);display_int(get_distanceF(),3);
	display_goto_xy(0,2);display_string("RD");display_goto_xy(2,2);display_int(get_distanceR(),3);
	display_goto_xy(0,3);display_string("WL");display_goto_xy(2,3);display_int(get_wPositionL(),4);
	display_goto_xy(0,4);display_string("WR");display_goto_xy(2,4);display_int(get_wPositionR(),4);
	display_goto_xy(0,5);display_string("PX");display_goto_xy(2,5);display_int(get_x(),3);
	display_goto_xy(0,6);display_string("PY");display_goto_xy(2,6);display_int(get_y(),3);
	//display_goto_xy(0,4);display_string("CL");display_goto_xy(2,4);display_int(dist_from_cell_cp(previous_cp(get_cardinal_point())),4);
	//display_goto_xy(0,5);display_string("CF");display_goto_xy(2,5);display_int(dist_from_cell_cp(get_cardinal_point()),4);
	//display_goto_xy(0,6);display_string("CR");display_goto_xy(2,6);display_int(dist_from_cell_cp(next_cp(get_cardinal_point())),3);
	//U8 color = get_color();
	//int result = color == NXT_COLOR_RED ? 1 : 0;
	//display_goto_xy(0,7);display_string("CL");display_goto_xy(2,7);display_int(result,3);
	display_goto_xy(0,7);display_string("W");display_goto_xy(2,7);display_int((int)get_w(),3);
	
	display_update();
}

void displayPID() {
	static int x=0,y=0;
	if(get_mov_order()!=MOVE_TO_XY) return;
	display_goto_xy(x,y);
	display_int(get_PID_error(),2);display_string(",");
	x=(x+3)%15;
	if(x==0) y=(y+1)%7;
	display_update();
}

void update_display() {
	switch(MODE_DISPLAY) {
		case DISPLAY_NONE: break;
		case DISPLAY_DATA: displayData(); break;
		case DISPLAY_MAP: display_map_debug(); break;
		case DISPLAY_PID: displayPID(); break;
		case DISPLAY_SCHED: display_scheduling(); break;
		default: break;
	}
}
