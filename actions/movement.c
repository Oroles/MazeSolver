#include "kernel.h"
#include "ecrobot_interface.h"
#include "utils/shared_variables.h"
#include "utils/utils.h"
#include "slam/localization.h"
#include "movement.h"

int __movement_order=STOP;
int __power=0;

U8 KD;
U8 KI;
U8 KP;
double error_int;
double error_dev;
double last_error;
double __target_w;

void set_orientation(orientation* orient,int direction) {
	if ( direction == 1 ) {
		switch(*orient) {
			case UP: *orient = RIGHT; break;
			case RIGHT: *orient = DOWN; break;
			case DOWN: *orient = LEFT; break;
			case LEFT: *orient = UP; break;
			default: break;
		}
	}
	else {
		switch(*orient) {
			case UP: *orient = LEFT; break;
			case LEFT: *orient = DOWN; break;
			case DOWN: *orient = RIGHT; break;
			case RIGHT: *orient = UP; break;
		}
	}
}

orientation access_orientation(int direction,int mode) {
	static orientation __orientation = UP;
	if(mode){ set_orientation(&__orientation,direction); return 0; }
	else return __orientation;
}

void change_orientation(int direction) {
	access_orientation(direction,1);
}

orientation get_orientation() {
	return access_orientation(0,0);
}


void init_PID(double target_w, U8 Kp, U8 Ki, U8 Kd) {
	error_int=0;
	error_dev=0;
	last_error=0;
	KP=Kp;
	KI=Ki;
	KD=Kd;
	__target_w=target_w;
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


void set_movement(int movement_order, int power) {
	GetResource(MovementOrder);
	__movement_order=movement_order;
	__power=power;
	ReleaseResource(MovementOrder);
}
int get_movement() {
	return __movement_order;
}

void stop() {
	set_movement(STOP,0);
}

void go_forward(int power) {
	init_PID(get_w(),10,1,10);
	set_movement(MOVE_FORWARD,power);
}

void turn_to_w(double w, int power) {
	init_PID(w,1,0,0);
	set_movement(TURN_TO_W,power);
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
	set_movement(TURN_RIGHT,power);
}

void turn_left(int power) {
	set_movement(TURN_LEFT,power);
}


void do_stop() {
	nxt_motor_set_speed(PORT_MOTOR_R,0,1);
	nxt_motor_set_speed(PORT_MOTOR_L,0,1);
}

void do_move_forward(int power) {
	int output=get_PID_output()/10;
	nxt_motor_set_speed( PORT_MOTOR_R, power - output, 1 );
	nxt_motor_set_speed( PORT_MOTOR_L, power + output, 1 );
}

void do_rotate_right(int power) {
	nxt_motor_set_speed(PORT_MOTOR_L,power,1);
	nxt_motor_set_speed(PORT_MOTOR_R,-power,1);
	change_orientation(1);
}

void do_rotate_left(int power) {
	nxt_motor_set_speed(PORT_MOTOR_L,-power,1);
	nxt_motor_set_speed(PORT_MOTOR_R,power,1);
	change_orientation(0);
}

void do_turn_to_w(int power) {
	int output= get_PID_output();
	
	if(output>10) output = power;
	else if ( output<-10) output=-power;
	else output= (int) (output*power/10);

	if(output<0 && output>-10) output=-10;
	else if(output>0 && output<10) output=10;

	if(last_error<=1 && last_error>=-1) {
		nxt_motor_set_speed( PORT_MOTOR_R, 0, 1 );
		nxt_motor_set_speed( PORT_MOTOR_L, 0, 1 );
		SetEvent(MainController, EndOfMovement);
	}
	else {
		nxt_motor_set_speed( PORT_MOTOR_R, -output, 1 );
		nxt_motor_set_speed( PORT_MOTOR_L, output, 1 );
	}
}

void do_movement() {
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
		default:
		break;
	}
}
