#include "utils.h"
#include "localization.h"
#include "mapping.h"
#include "movement.h"
#include "shared_variables.h"
#include "main_controller.h"

int find_next_goal() {
	int tmp_goal=-1;
	for(int direction=NORTH ; is_cp(direction) ; direction=next_cp(direction)) {
		if(is_wall_in_direction(direction)) tmp_goal=direction;
		if(is_visited_in_direction(direction)) return direction;
	}
	return tmp_goal;
}

void turn_to(int orientation) {
	if(get_cardinal_point()!=orientation) rotate_right(50);
	ClearEvent(NewCardinalPoint);
	while(get_cardinal_point()!=orientation) {
		WaitEvent(NewCardinalPoint);
		ClearEvent(NewCardinalPoint);
	}
	stop();
}

void goto_cp(int goal){
	turn_to(goal);
	move_forward(50);
}

int main_step() {
	if(get_color()!=NXT_COLOR_GREEN) {
		S8 goal;
		goal=find_next_goal();

		goto_cp(goal);
		return 0;
	}
	else return 1;
}
