#ifndef _ORD_H
#define _ORD_H

// Movement Orders
#define STOP 			0
#define MOVE_FORWARD 	1
#define TURN_RIGHT 		2
#define TURN_LEFT 		3
#define TURN_TO_W 		4
#define MOVE_TO_XY 		5

DeclareResource(MovementOrder);

int get_mov_order();
int get_mov_power();
int get_mov_target_w();
int get_mov_target_x();
int get_mov_target_y();
int is_mov_ended();

void set_mov_target_w(double target_w);
void end_of_mov();

void stop();
void go_forward(int power);
void turn_right(int power);
void turn_left(int power);
void turn_to_w(double w, int power);
void turn_to_cp(int cp, int power);
void move_to_xy(double x, double y, int power);
void move_to_cell(int cx, int cy, int power);

#endif
