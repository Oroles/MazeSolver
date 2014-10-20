#include "kernel.h"
#include "ecrobot_interface.h"
#include "shared_variables.h"
#include "movement.h"

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

	/*S32 left_distance = get_distanceL();
	S32 right_distance = get_distanceR();

	if ( left_distance > ( right_distance + THRESHOLD_DISTANCE ) )
	{
		nxt_motor_set_speed(PORT_MOTOR_R,power+power/2,1);
	}
	if ( right_distance > ( left_distance + THRESHOLD_DISTANCE ) )
	{
		nxt_motor_set_speed(PORT_MOTOR_L,power+power/2,1);
	}*/
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
