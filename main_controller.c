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
struct node* commands = NULL;

int get_next_direction() {
	int direction;
	struct node* command = remove_first_node(&commands);
	direction = direction_of_next_cell(get_x(),get_y(),command->x,command->y);
	free(command);

	return direction;
}

int find_next_goal() {
	int direction=NORTH;
	do {
		if(is_wall_in_direction(direction,get_x(),get_y())==NO_WALL) {
			if(is_visited_in_direction(direction,get_x(),get_y())==FALSE) {
				free_list(commands);
				return direction;
			}
		}
		direction=next_cp(direction);
	}while(direction!=NORTH);

	// If the next position is known, try to go in its direction
	if(commands != NULL) {
		int direction=get_next_direction();
		if (is_wall_in_direction(direction,get_x(),get_y()) != NO_WALL )
			free_list(commands);
		else
			return direction;
	}
	
	// If the next position is still unknown, try to find a new goal
	commands = find_unvisited_cell(get_x(),get_y());

	if (commands==NULL) { // If no unvisited cell, try to go back to the starting position
		commands = find_shortest_path(get_x(),get_y(),0,0);
		if (commands == NULL) //It means that there is not path from the current cell to the starting cell (0,0), or it's the starting cell
			return -1;
	}

	return get_next_direction();
}

void goto_cp(int goal){
	turn_to_cp(goal,20);
	WaitEvent(EndOfMovement);
	ClearEvent(EndOfMovement);
	move_to_cell(__next_goal_x,__next_goal_y,30);
}

int main_step() {
	if(!is_stop_color()) {
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
