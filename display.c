#include "kernel.h"
#include "ecrobot_interface.h"
#include "shared_variables.h"
#include "localization.h"
#include "mapping.h"
#include "display.h"

#define DISPLAY_NONE 	0
#define DISPLAY_DATA 	1
#define DISPLAY_MAP 	2

#define MODE_DISPLAY 	1

void displayData() {
	display_clear(0);
	display_goto_xy(0,0);display_string("LD");display_goto_xy(2,0);display_int(get_distanceL(),3);
	display_goto_xy(0,1);display_string("FD");display_goto_xy(2,1);display_int(get_distanceF(),3);
	display_goto_xy(0,2);display_string("RD");display_goto_xy(2,2);display_int(get_distanceR(),3);
	display_goto_xy(0,3);display_string("WL");display_goto_xy(2,3);display_int(get_wPositionL(),4);
	display_goto_xy(0,4);display_string("WR");display_goto_xy(2,4);display_int(get_wPositionR(),4);
	display_goto_xy(0,5);display_string("PX");display_goto_xy(2,5);display_int(get_x(),3);
	display_goto_xy(0,6);display_string("PY");display_goto_xy(2,6);display_int(get_y(),3);
	U8 color = get_color();
	int result = color == NXT_COLOR_RED ? 1 : 0;
	display_goto_xy(0,7);display_string("CL");display_goto_xy(2,7);display_int(result,3);
	
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
		default: break;
	}
}
