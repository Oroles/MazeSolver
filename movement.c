#include "movement.h"
#include "shared_variables.h"

void stop() {
	nxt_motor_set_speed(PORT_MOTOR_R,0,1);
	nxt_motor_set_speed(PORT_MOTOR_L,0,1);
}

void move_forward(U32 power) {
	nxt_motor_set_speed(PORT_MOTOR_R,20,0);
	nxt_motor_set_speed(PORT_MOTOR_L,20,0);
}

void rotate_right() {

}

void rotate_left() {

}