#ifndef _PARAMS_H
#define _PARAMS_H

// General statics
#define PI 			3.14159265
#define RAD 		(PI/180.0) 	// Conversion factor that translate angles from degrees to radians

// About the robot
#define W_DIST 		13 			// The distance between the wheels (in mm)
#define W_DIAM		56 			// Nominal wheel diameter (in mm)
#define PULSES 		360 		// Encoder resolution (in pulses per revolution)
#define CONV 		(PI*W_DIAM/PULSES)		// Conversion factor that translates encoder pulses into linear wheel displacement

// about the map
#define MAP_RES 	200 		// Resolution of the map (in mm) : size of the side of squares used to represent the maze

DeclareResource(UpdateLocker);

int get_x();
int get_y();
int get_d();
void update_localization();

#endif
