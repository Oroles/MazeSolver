#include "utils/utils.h"
#include "utils/shared_variables.h"
#include "slam/localization.h"
#include "slam/mapping.h"
#include "actions/movement.h"
#include "main_controller.h"

int find_next_goal() {
	int tmp_goal=-1;
	int direction=NORTH;
	do {
		if(is_wall_in_direction(direction,get_x(),get_y())==NO_WALL) {
			if(!is_visited_in_direction(direction,get_x(),get_y())) return direction;
			else tmp_goal=direction;
		}
		direction=next_cp(direction);
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
	static int __next_goal_x=0;
	static int __next_goal_y=0;
	if(get_color()!=NXT_COLOR_GREEN) {
		if(get_x()==__next_goal_x && get_y()==__next_goal_y) {
			stop();

			// If we're not in direction of one of the four cardinal points, we turn.
			if(!is_cp(get_cp())) {
				turn_to_cp(next_cp(get_cp()),20);
				WaitEvent(EndOfMovement);
				ClearEvent(EndOfMovement);
			}

			// Find the next goal, then go in its direction
			int goal=find_next_goal();

			if(goal>=0) {
				coord_for_cp_square(goal,&__next_goal_x,&__next_goal_y);
				goto_cp(goal);
			}
		}
		return 0;
	}
	else return 1;
}
