#include "utils/utils.h"
#include "slam/localization.h"
#include "movement.h"
#include "commands.h"

int __movement_order=STOP;
int __power=0;
double __target_x;
double __target_y;


int get_movement() {
	return __movement_order;
}

void stop() {
	GetResource(MovementOrder);
	__movement_order=STOP;
	ReleaseResource(MovementOrder);
}

void go_forward(int power) {
	GetResource(MovementOrder);
	__movement_order=MOVE_FORWARD;
	__power=power;
	init_PID(get_w(),1,0.5,0.2);
	ReleaseResource(MovementOrder);
}

void move_to_xy(double x, double y, int power) {
	GetResource(MovementOrder);
	__movement_order=MOVE_TO_XY;
	__power=power;
	__target_x=x;
	__target_y=y;
	init_PID(angle_to_reach(get_realX(),get_realY(),__target_x,__target_y),1,0.5,0.2);
	ReleaseResource(MovementOrder);
}
void move_to_cell(int cx, int cy, int power) {
	double x,y;
	cell_center(cx,cy,&x,&y);
	move_to_xy(x,y,power);
}

void turn_to_w(double w, int power) {
	GetResource(MovementOrder);
	__movement_order=TURN_TO_W;
	__power=power;
	init_PID(w,1,0,0);
	ReleaseResource(MovementOrder);
}
void turn_to_cp(int cp, int power) {
	switch(cp) {
		case NORTH:
		turn_to_w(90,power); break;
		case EAST:
		turn_to_w(0,power); break;
		case SOUTH:
		turn_to_w(270,power); break;
		case WEST:
		turn_to_w(180,power); break;
	}
}

void turn_right(int power) {
	GetResource(MovementOrder);
	__movement_order=TURN_RIGHT;
	__power=power;
	ReleaseResource(MovementOrder);
}

void turn_left(int power) {
	GetResource(MovementOrder);
	__movement_order=TURN_LEFT;
	__power=power;
	ReleaseResource(MovementOrder);
}

void do_move_to_xy() {
	double x=get_realX();
	double y=get_realY();

	set_target_w(angle_to_reach(x,y,__target_x,__target_y));

	if(__target_x-x<=1 && x-__target_x<=1 &&
		__target_y-y<=1 && y-__target_y<=1) {
		do_stop();
		SetEvent(MainController, EndOfMovement);
	}
	else
		do_move_forward(__power);
}

void do_movement() {
	GetResource(MovementOrder);
	switch(__movement_order) {
		case STOP:
		do_stop();
		break;
		case MOVE_FORWARD:
		do_move_forward(__power);
		break;
		case TURN_RIGHT:
		do_rotate_right(__power);
		break;
		case TURN_LEFT:
		do_rotate_left(__power);
		break;
		case TURN_TO_W:
		do_turn_to_w(__power);
		break;
		case MOVE_TO_XY:
		do_move_to_xy();
		break;
		default:
		break;
	}
	ReleaseResource(MovementOrder);
}
