#include "kernel.h"
#include "ecrobot_interface.h"
#include "utils/shared_variables.h"
#include "utils/utils.h"
#include "slam/localization.h"
#include "movement.h"

U8 KD;
U8 KI;
U8 KP;
double error_int;
double error_dev;
double last_error;
double __target_w;
double ramp_up = 0;


void init_PID(double target_w, U8 Kp, U8 Ki, U8 Kd) {
	error_int=0;
	error_dev=0;
	last_error=0;
	KP=Kp;
	KI=Ki;
	KD=Kd;
	__target_w=target_w;
}

void set_target_w(double target_w) {
	__target_w=target_w;
}

double get_target_w() {
	return __target_w;
}

int get_PID_output() {
	double error = get_w() - __target_w;
	if ( error > 180 ) error -=360;
	else if( error < -180) error +=360;

	error_dev = error - last_error;
	error_int += error;
	last_error = error;
	return (int) (KP * error + KI * error_int + KD * error_dev);
}


void do_stop() {
	nxt_motor_set_speed(PORT_MOTOR_R,0,1);
	nxt_motor_set_speed(PORT_MOTOR_L,0,1);
	ramp_up =0;
}

void do_move_forward(int power) {
	int output=get_PID_output();
	if (ramp_up < power){ramp_up += 5;}
	else {ramp_up = power;}
	nxt_motor_set_speed( PORT_MOTOR_R, ramp_up - output, 1 );
	nxt_motor_set_speed( PORT_MOTOR_L, ramp_up + output, 1 );
}

void do_turn(int power) {
	int output= get_PID_output();
	ramp_up = 0;
	if(output>10) output = power;
	else if ( output<-10) output=-power;
	else output= (int) (output*power/10);

	if(output<0 && output>-10) output=-10;
	else if(output>0 && output<10) output=10;

	nxt_motor_set_speed( PORT_MOTOR_R, -output, 1 );
	nxt_motor_set_speed( PORT_MOTOR_L, output, 1 );
}

void do_rotate_right(int power) {
	nxt_motor_set_speed(PORT_MOTOR_L,power,1);
	nxt_motor_set_speed(PORT_MOTOR_R,-power,1);
}

void do_rotate_left(int power) {
	nxt_motor_set_speed(PORT_MOTOR_L,-power,1);
	nxt_motor_set_speed(PORT_MOTOR_R,power,1);
}
