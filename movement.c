#include "kernel.h"
#include "ecrobot_interface.h"
#include "shared_variables.h"
#include "movement.h"

void stop() {
	nxt_motor_set_speed(PORT_MOTOR_R,0,1);
	nxt_motor_set_speed(PORT_MOTOR_L,0,1);
}

void move_forward(U32 power) {
	nxt_motor_set_speed(PORT_MOTOR_R,power,1);
	nxt_motor_set_speed(PORT_MOTOR_L,power,1);

	S32 left_distance = get_distanceL();
	S32 right_distance = get_distanceR();

	if ( left_distance > ( right_distance + THRESHOLD_DISTANCE ) )
	{
		nxt_motor_set_speed(PORT_MOTOR_R,power+power/2,1);
	}
	if ( right_distance > ( left_distance + THRESHOLD_DISTANCE ) )
	{
		nxt_motor_set_speed(PORT_MOTOR_L,power+power/2,1);
	}
}

void rotate_right(U32 power) {
	nxt_motor_set_speed(PORT_MOTOR_L,power,1);
	nxt_motor_set_speed(PORT_MOTOR_R,-power,1);
}

void rotate_left(U32 power) {
	nxt_motor_set_speed(PORT_MOTOR_L,-power,1);
	nxt_motor_set_speed(PORT_MOTOR_R,power,1);
}
