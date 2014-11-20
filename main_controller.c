#include "utils/utils.h"
#include "utils/shared_variables.h"
#include "slam/localization.h"
#include "slam/mapping.h"
#include "slam/path_finding.h"
#include "slam/node.h"
#include "actions/commands.h"
#include "main_controller.h"

int __next_goal_x=0;
int __next_goal_y=0;

int find_next_goal() {
	int direction=NORTH;
	do {
		if(is_wall_in_direction(direction,get_x(),get_y())==NO_WALL) {
			if(is_visited_in_direction(direction,get_x(),get_y())==FALSE) return direction;
		}
		direction=next_cp(direction);
	}while(direction!=NORTH);
	struct node* next_cell = find_unvisited_cell(get_x(),get_y());
	direction = direction_of_next_cell(get_x(),get_y(),next_cell->x,next_cell->y);
	free( next_cell );
	return direction;
}

void goto_cp(int goal){
	turn_to_cp(goal,20);
	WaitEvent(EndOfMovement);
	ClearEvent(EndOfMovement);
	move_to_cell(__next_goal_x,__next_goal_y,30);
}

int is_stop() {
	if ( get_color() == NXT_COLOR_GREEN && get_color_counter()>3) return TRUE;
	return FALSE;
}

int main_step() {
	if(!is_stop()) {
		if(get_x()==__next_goal_x && get_y()==__next_goal_y) {
			stop();

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

void main_end() {
	stop();
}
