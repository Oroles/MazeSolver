#ifndef _LOC_H
#define _LOC_H

#include "kernel.h"
#include "ecrobot_interface.h"

#define CARD_PRECISION 2 	// The 4 cardinal points North, East, South and West (for direction of the robot) will be precise with +/- CARD_PRECISION degrees

DeclareResource(UpdateLocker);

int get_x();
double get_realX();
int get_y();
double get_realY();
double get_w();
int get_cardinal_point();
void update_localization();

#endif
