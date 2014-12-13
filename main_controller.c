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
int goal;

int get_direction(struct node** commands) {
	int direction;
	struct node* command = remove_first_node(commands);
	direction = direction_of_next_cell(get_x(),get_y(),command->x,command->y);
	free( command );

	if ( is_wall_in_direction(direction,get_x(),get_y()) != NO_WALL ) {
		direction = -1;
		free_list( commands );
	}

	return direction;
}

int find_next_goal() {
	int direction=NORTH;
	do {
		if(is_wall_in_direction(direction,get_x(),get_y())==NO_WALL) {
			if(is_visited_in_direction(direction,get_x(),get_y())==FALSE) {
				free_list(&commands);
				return direction;
			}
		}
		direction=next_cp(direction);
	}while(direction!=NORTH);
	if ( commands == NULL ) {
		commands = find_unvisited_cell(get_x(),get_y());
	}
	direction = get_direction(&commands);

	if ( direction == -1 ) { //Means that there is no stop position so we try to get back to the starting position
		free_list(&commands);
		if ( get_x() == 0 && get_y() == 0 ) { //Reach the starting node and nothing else to explore
			return direction;
		}
		else {
			commands = find_shortest_path(get_x(),get_y(),0,0);
		}

		if ( commands == NULL ) { //It means that there is not path from the current nod to the starting node(0,0)
			direction = -1;
		}
		else {
			direction = get_direction(&commands);
		}
	}

	return direction;
}

int get_goal() {
	return goal;
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
			goal=find_next_goal();

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
