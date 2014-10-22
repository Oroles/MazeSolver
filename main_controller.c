#include "utils.h"
#include "localization.h"
#include "mapping.h"
#include "movement.h"
#include "shared_variables.h"
#include "main_controller.h"

int find_next_goal() {
	int tmp_goal=-1;
	int direction=NORTH;
	do {
		if(is_wall_in_direction(direction)==NO_WALL) {
			if(!is_visited_in_direction(direction)) return direction;
			else tmp_goal=direction;
		}
		direction=next_cp(direction);
	}while(direction!=NORTH);
	return tmp_goal;
}

void turn_to(int orientation) {
	if(get_cardinal_point()!=orientation) {
		rotate_right(20);
		while(get_cardinal_point()!=orientation) {
			WaitEvent(NewCardinalPoint);
			ClearEvent(NewCardinalPoint);
		}
		stop();
	}
}

void goto_cp(int goal){
	turn_to(goal);
	move_forward(40);
}

int main_step() {
	if(get_color()!=NXT_COLOR_GREEN) {
		int goal;
		goal=find_next_goal();

		if(goal>=0) goto_cp(goal);
		return 0;
	}
	else return 1;
}
