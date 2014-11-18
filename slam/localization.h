#ifndef _LOC_H
#define _LOC_H

#include "kernel.h"
#include "ecrobot_interface.h"

#define CARD_PRECISION 5 	// The 4 cardinal points North, East, South and West (for direction of the robot) will be precise with +/- CARD_PRECISION degrees
#define L_CALIBRATION  (1.00147814) 
#define R_CALIBRATION  (0.99852185)

DeclareResource(UpdateLocker);

int get_x();
double get_realX();
int get_y();
double get_realY();
double get_w();
int get_cardinal_point();
void init_localization();
void update_localization();
int is_inside_square(double rx, double ry, int side);

#endif
