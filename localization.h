#ifndef _LOC_H
#define _LOC_H

#include "kernel.h"
#include "ecrobot_interface.h"

#define PI 			3.14159265
#define RAD 		(PI/180.0) 	// Conversion factor that translate angles from degrees to radians
#define W_DIST 		13 			// The distance between the wheels (mm)
#define W_DIAM		56 			// Nominal wheel diameter (in mm)
#define PULSES 		360 		// Encoder resolution (in pulses per revolution)
#define CONV 		(PI*W_DIAM/PULSES)		// Conversion factor that translates encoder pulses into linear wheel displacement

DeclareResource(UpdateLocker);

int get_x();
int get_y();
int get_d();
void update_localization();

#endif
