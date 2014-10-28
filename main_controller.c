#include "utils/utils.h"
#include "utils/shared_variables.h"
#include "slam/localization.h"
#include "slam/mapping.h"
#include "actions/movement.h"
#include "main_controller.h"

int find_next_goal() {
	int tmp_goal=-1;
	int direction=NORTH,pos=4;
	do {
		if(is_wall_in_direction(direction,get_x(),get_y())==NO_WALL) {
			display_goto_xy(pos,0);display_string("NO WALL IN ");display_cp(direction);
			if(!is_visited_in_direction(direction,get_x(),get_y())) return direction;
			else tmp_goal=direction;
		}
		direction=next_cp(direction);
		pos++;
	}while(direction!=NORTH);
	return tmp_goal;
}

void goto_cp(int goal){
	turn_to_cp(goal,20);
	WaitEvent(EndOfMovement);
	ClearEvent(EndOfMovement);
	go_forward(40);
}

int main_step() {
	if(get_color()!=NXT_COLOR_GREEN) {
		int goal;
		goal=find_next_goal();

		//if(goal!=-1) goto_cp(goal);
		return 0;
	}
	else return 1;
}
