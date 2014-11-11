#include "utils/utils.h"
#include "slam/localization.h"
#include "pid_control.h"
#include "commands.h"

U8 __movement_order=STOP;
int __power=0;
U8 __movement_ended=FALSE;
double __target_w;
double __target_x;
double __target_y;

U8 get_mov_order() { return __movement_order; }
int get_mov_power() { return __power; }
int get_mov_target_w() { return __target_w; }
int get_mov_target_x() { return __target_x; }
int get_mov_target_y() { return __target_y; }
U8 is_mov_ended() { return __movement_ended; }

void set_mov_target_w(double target_w) { __target_w=target_w; }
void end_of_mov() { __movement_ended=TRUE; }

void stop() {
	GetResource(MovementOrder);
	__movement_order=STOP;
	ReleaseResource(MovementOrder);
}

void go_forward(int power) {
	GetResource(MovementOrder);
	__movement_order=MOVE_FORWARD;
	__power=power;
	__target_w=get_w();
	init_PID(1,0.5,0.2);
	ReleaseResource(MovementOrder);
}

void move_to_xy(double x, double y, int power) {
	GetResource(MovementOrder);
	__movement_order=MOVE_TO_XY;
	__power=power;
	__movement_ended=FALSE;
	__target_x=x;
	__target_y=y;
	__target_w=angle_to_reach(get_realX(),get_realY(),__target_x,__target_y);
	init_PID(1.2,0.3,0.6);
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
	__movement_ended=FALSE;
	__target_w=w;
	init_PID(1,0,0);
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
