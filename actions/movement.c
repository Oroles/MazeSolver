#include "kernel.h"
#include "ecrobot_interface.h"
#include "utils/shared_variables.h"
#include "utils/utils.h"
#include "slam/localization.h"
#include "pid_control.h"
#include "commands.h"
#include "movement.h"

#define POS_RES 4

double ramp_up = 0;

void do_stop() {
	stop_PID();
	nxt_motor_set_speed(PORT_MOTOR_R,0,1);
	nxt_motor_set_speed(PORT_MOTOR_L,0,1);
	ramp_up =0;
}

void do_move_forward(int power) {
	int output=get_PID_output();
	if(power<0) {
		if (ramp_up > power)ramp_up -= 5;
		else ramp_up = power;
		if(output<ramp_up) output=ramp_up;
		else if(output>-ramp_up) output=-ramp_up;
	}
	else {
		if (ramp_up < power) ramp_up += 5;
		else ramp_up = power;
		if(output>ramp_up) output=ramp_up;
		else if(output<-ramp_up) output=-ramp_up;
	}
	nxt_motor_set_speed( PORT_MOTOR_R, ramp_up - output, 1 );
	nxt_motor_set_speed( PORT_MOTOR_L, ramp_up + output, 1 );
}

void do_turn(int power) {
	int output= get_PID_output();
	ramp_up = 0;
	
	if(output>0) {
		if(output>power) output=power;
		else if(output<10 && output) output=10;
	}
	else {
		if(output<-power) output=-power;
		else if(output>-10) output=-10;
	}

	nxt_motor_set_speed( PORT_MOTOR_R, -output, 1 );
	nxt_motor_set_speed( PORT_MOTOR_L, output, 1 );
}

void do_rotate_right(int power) {
	stop_PID();
	nxt_motor_set_speed(PORT_MOTOR_L,power,1);
	nxt_motor_set_speed(PORT_MOTOR_R,-power,1);
}

void do_rotate_left(int power) {
	stop_PID();
	nxt_motor_set_speed(PORT_MOTOR_L,-power,1);
	nxt_motor_set_speed(PORT_MOTOR_R,power,1);
}

void do_turn_to_w(int power) {
	double error= get_w()-get_mov_target_w();

	if(error<=1 && error>=-1) {
		end_of_mov();
		do_stop();
		SetEvent(MainController, EndOfMovement);
	}
	else {
		do_turn(power);
	}
}

void do_move_to_xy(int target_x,int target_y,int power) {
	if(!is_mov_ended()) {
		double x=get_realX();
		double y=get_realY();
		double w_to_reach=angle_to_reach(x,y,target_x,target_y);
		double wdiff=get_w()-w_to_reach;
		if(target_x-x<=POS_RES+40 && x-target_x<=POS_RES+40 &&
			target_y-y<=POS_RES+40 && y-target_y<=POS_RES+40) {
			set_mov_target_w(w_to_reach);
			do_move_forward(15);
			if(target_x-x<=POS_RES && x-target_x<=POS_RES &&
				target_y-y<=POS_RES && y-target_y<=POS_RES) {
				end_of_mov();
				do_stop();
				SetEvent(MainController, EndOfMovement);
			}
		}
		else {
			if(wdiff>5 || wdiff<-5)
				set_mov_target_w(w_to_reach);
			do_move_forward(power);
		}
	}
}

void do_movement() {
	GetResource(MovementOrder);
	switch(get_mov_order()) {
		case STOP:
		do_stop(get_mov_power());
		break;
		case MOVE_FORWARD:
		do_move_forward(get_mov_power());
		break;
		case TURN_RIGHT:
		do_rotate_right(get_mov_power());
		break;
		case TURN_LEFT:
		do_rotate_left(get_mov_power());
		break;
		case TURN_TO_W:
		do_turn_to_w(get_mov_power());
		break;
		case MOVE_TO_XY:
		do_move_to_xy(get_mov_target_x(),get_mov_target_y(),get_mov_power());
		break;
		default:
		break;
	}
	ReleaseResource(MovementOrder);
}
