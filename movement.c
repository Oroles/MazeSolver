#include "kernel.h"
#include "ecrobot_interface.h"
#include "shared_variables.h"
#include "movement.h"
#include "localization.h"

int __movement_order=STOP;
U32 __power=0;

int KD = 10;
int KI = 1;
int KP = 10;
int error_int;
int error_dev;
double last_error;
double initial_w;

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




void set_movement(int movement_order, U32 power) {
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

void go_forward(U32 power) {
	error_int=0;
	error_dev=0;
	last_error=0;

	initial_w=get_w();
	set_movement(MOVE_FORWARD,power);
}

void turn_right(U32 power) {
	set_movement(TURN_RIGHT,power);
}

void turn_left(U32 power) {
	set_movement(TURN_LEFT,power);
}



void do_stop() {
	nxt_motor_set_speed(PORT_MOTOR_R,0,1);
	nxt_motor_set_speed(PORT_MOTOR_L,0,1);
}

void do_move_forward(U32 power) {
	nxt_motor_set_speed(PORT_MOTOR_R,power,1);
	nxt_motor_set_speed(PORT_MOTOR_L,power,1);

	double error = get_w() - initial_w;
	if ( error > 180 ) error -=360;
	else if( error < -180) error +=360;

	error_dev = error - last_error;
	error_int += error;
	int output = KP * error + KI * error_int + KD * error_dev;
	last_error = error;

	nxt_motor_set_speed( PORT_MOTOR_R, power - output, 1 );
	nxt_motor_set_speed( PORT_MOTOR_L, power + output, 1 );
}

void do_rotate_right(U32 power) {
	nxt_motor_set_speed(PORT_MOTOR_L,power,1);
	nxt_motor_set_speed(PORT_MOTOR_R,-power,1);
	change_orientation(1);
}

void do_rotate_left(U32 power) {
	nxt_motor_set_speed(PORT_MOTOR_L,-power,1);
	nxt_motor_set_speed(PORT_MOTOR_R,power,1);
	change_orientation(0);
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
		default:
		break;
	}
}
