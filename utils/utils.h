#ifndef _UTILS_H
#define _UTILS_H

#include "kernel.h"
#include "kernel_id.h"

// General statics
#define PI 			3.14159265
#define RAD 		(PI/180.0) 	// Conversion factor that translate angles from degrees to radians

// About the robot
#define W_DIST 		120 		// The distance between the wheels (in mm)
#define W_DIAM		56 			// Nominal wheel diameter (in mm)
#define W_PERI		178.0 		// Nominal wheel diameter (in mm)
#define PULSES 		360 		// Encoder resolution (in pulses per revolution)
#define CONV 		(W_PERI/PULSES)		// Conversion factor that translates encoder pulses into linear wheel displacement

// About the map
#define MAP_RES 	300 	// Resolution of the map (in mm) : size of the side of squares used to represent the maze
#define MAP_WIDTH 	15 		// Width of the map
#define MAP_HEIGHT 	7 		// Height of the map

// Cardinal points
#define NORTH 	0 	// North
#define NO_EA 	1 	// North East
#define EAST 	2 	// East
#define SO_EA 	3 	// South East
#define SOUTH 	4 	// South
#define SO_WE 	5 	// South West
#define WEST 	6 	// West
#define NO_WE 	7 	// North West

// Returns values for the function is_wall_in_direction(int direction)
#define UNKNOWN 0
#define NO_WALL 1
#define IS_WALL 2
#define ERROR   3


/* OSEK declarations */
DeclareEvent(NewDiscovery);
DeclareEvent(EndOfMovement);

DeclareCounter(SysTimerCnt);
DeclareTask(ColorReader);
DeclareTask(DistanceReader);
DeclareTask(WheelsPositionReader);
DeclareTask(Localization);
DeclareTask(Mapping);
DeclareTask(Movement);
DeclareTask(MainController);

/* Function declarations */
int next_cp(int cp);
int previous_cp(int cp);
int is_cp(int val);
int coord_for_cp_square(int cp, int *x, int *y);
void display_cp(int cp);

#endif
