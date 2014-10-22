#ifndef _MOV_H
#define _MOV_H

#define THRESHOLD_DISTANCE  2
#define EXPECTED_VALUE 		7
#define PROPORTIONAL		3

// Movement Orders
#define STOP 			0
#define MOVE_FORWARD 	1
#define TURN_RIGHT 		2
#define TURN_LEFT 		3

typedef enum { UP, RIGHT, DOWN, LEFT } orientation;

DeclareResource(MovementOrder);

int get_movement();

void stop();
void go_forward(U32 power);
void turn_right(U32 power);
void turn_left(U32 power);

void do_movement();

orientation get_orientation();


#endif
