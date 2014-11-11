#ifndef _PID_H
#define _PID_H

void init_PID(double Kp, double Ki, double Kd);
int get_PID_output();
int get_PID_error();

void calculate_PID_output(double target_w);
void stop_PID();

#endif
