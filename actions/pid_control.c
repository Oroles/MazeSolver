#include "kernel.h"
#include "ecrobot_interface.h"
#include "utils/utils.h"
#include "slam/localization.h"
#include "movement.h"

int pid_needed=FALSE;

U8 KD;
U8 KI;
U8 KP;
double error_int;
double error_dev;
double last_error;
int pid_output=0;

void init_PID(U8 Kp, U8 Ki, U8 Kd) {
	pid_output=0;
	error_int=0;
	error_dev=0;
	last_error=0;
	KP=Kp;
	KI=Ki;
	KD=Kd;
	pid_needed=TRUE;
}
void stop_PID() {
	pid_output=0;
	pid_needed=FALSE;
}

void calculate_PID_output(double target_w) {
	if(pid_needed) {
		double error = get_w() - target_w;
		if ( error > 180 ) error -=360;
		else if( error < -180) error +=360;

		error_dev = error - last_error;
		error_int += error;
		last_error = error;
		pid_output = (int) (KP * error + KI * error_int + KD * error_dev);
	}
}

int get_PID_output() {
	return pid_output;
}

int get_PID_error() {
	return (int) last_error;
}
