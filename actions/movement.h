#ifndef _MOV_H
#define _MOV_H

#define EXPECTED_VALUE 		7
#define PROPORTIONAL		3

void init_PID(double target_w, U8 Kp, U8 Ki, U8 Kd);
void set_target_w(double target_w);

void do_stop();
void do_move_forward(int power);
void do_rotate_right(int power);
void do_rotate_left(int power);
void do_turn_to_w(int power);

#endif
