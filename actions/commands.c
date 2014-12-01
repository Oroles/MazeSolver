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
U32 __timeout;

U8 get_mov_order() { return __movement_order; }
int get_mov_power() { return __power; }
int get_mov_target_w() { return __target_w; }
int get_mov_target_x() { return __target_x; }
int get_mov_target_y() { return __target_y; }
U32 get_mov_timeout() { return __timeout; }
U8 is_mov_ended() { return __movement_ended; }

void set_mov_target_w(double target_w) { __target_w=target_w; }
void end_of_mov() { __movement_ended=TRUE; }

void stop() {
	__movement_order=STOP;
}

void go_forward(int power) {
	__movement_order=MOVE_FORWARD;
	__power=power;
	__target_w=get_w();
	init_PID(1,0.1,0.4);
}

void move_to_xy(double x, double y, int power) {
	__movement_order=MOVE_TO_XY;
	__power=power;
	__movement_ended=FALSE;
	__target_x=x;
	__target_y=y;
	double w_to_reach;
	if(power>=0) w_to_reach=angle_to_reach(get_realX(),get_realY(),__target_x,__target_y);
	else w_to_reach=angle_to_reach(__target_x,__target_y,get_realX(),get_realY());
	init_PID(1,0.1,0.4);
}
void move_to_cell(int cx, int cy, int power) {
	double x,y;
	cell_center(cx,cy,&x,&y);
	move_to_xy(x,y,power);
}

void stop_during_period(U32 ms) {
	__movement_order=STOP_MS;
	__timeout=systick_get_ms()+ms;
	__movement_ended=FALSE;
}

void turn_to_w(double w, int power) {
	__movement_order=TURN_TO_W;
	__power=power;
	__movement_ended=FALSE;
	__target_w=w;
	init_PID(1,0,0);
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
	__movement_order=TURN_RIGHT;
	__power=power;
}

void turn_left(int power) {
	__movement_order=TURN_LEFT;
	__power=power;
}
