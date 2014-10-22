#include "kernel.h"
#include "ecrobot_interface.h"
#include "shared_variables.h"
#include "movement.h"
#include "localization.h"

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

void stop() {
	nxt_motor_set_speed(PORT_MOTOR_R,0,1);
	nxt_motor_set_speed(PORT_MOTOR_L,0,1);
}

void move_forward(U32 power) {
	nxt_motor_set_speed(PORT_MOTOR_R,power,1);
	nxt_motor_set_speed(PORT_MOTOR_L,power,1);

	double error = get_w();
	if ( error > 180 ) {
		error = error - 360;
	}

	error_dev = error - last_error;
	error_int += error;
	int output = KP * error + KI * error_int + KD * error_dev;
	last_error = error;

	nxt_motor_set_speed( PORT_MOTOR_R, power - output, 1 );
	nxt_motor_set_speed( PORT_MOTOR_L, power + output, 1 );

	display_clear(0);
	/*display_goto_xy(0,0);
	display_int(left_distance,3);
	display_goto_xy(0,1);
	display_int(right_distance,3);
	display_goto_xy(0,2);
	display_int(left_error, 3);
	display_goto_xy(0,3);
	display_int(right_error, 3);*/
	display_goto_xy(0,4);
	display_int(error, 4);
	display_update();
}

void rotate_right(U32 power) {
	nxt_motor_set_speed(PORT_MOTOR_L,power,1);
	nxt_motor_set_speed(PORT_MOTOR_R,-power,1);
	change_orientation(1);
}

void rotate_left(U32 power) {
	nxt_motor_set_speed(PORT_MOTOR_L,-power,1);
	nxt_motor_set_speed(PORT_MOTOR_R,power,1);
	change_orientation(0);
}
