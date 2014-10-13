#ifndef _LOC_H
#define _LOC_H

#include "kernel.h"
#include "ecrobot_interface.h"

#define D	111 	// The distance between the wheels (mm)
#define R	20 		// The radius of a wheel (mm)

DeclareResource(UpdateLocker);

int get_x();
int get_y();
int get_d();
void update_localization();

#endif
