#ifndef _UTILS_H
#define _UTILS_H

#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

/* PORTS */
#define PORT_COLOR NXT_PORT_S4
#define PORT_DISTANCE_R NXT_PORT_S1
#define PORT_DISTANCE_F NXT_PORT_S2
#define PORT_DISTANCE_L NXT_PORT_S3
#define PORT_MOTOR_R NXT_PORT_C
#define PORT_MOTOR_L NXT_PORT_B

// General statics
#define PI 			3.14159265
#define RAD 		(PI/180.0) 	// Conversion factor that translate angles from degrees to radians

// About the robot
#define W_BASE_CALIBRATION (0.94596046)
#define W_DIST		(W_BASE_CALIBRATION*125) // The distance between the wheels (in mm)
#define W_DIAM		56 			// Nominal wheel diameter (in mm)
#define W_PERI		176.5 		// Nominal wheel perimeter (in mm)
#define PULSES 		360 		// Encoder resolution (in pulses per revolution)
#define CONV 		(W_PERI/PULSES)		// Conversion factor that translates encoder pulses into linear wheel displacement
// Distance sensors
#define MIN_DISTANCE 		5
#define MAX_DISTANCE 		255
#define CENTER_TO_FRONT 	3 // Distance from the front sensor to the center of the three sensors (center of the robot)
#define CENTER_TO_SIDES 	6 // Distance from one of the side sensors to the center of the three sensors (center of the robot)

// About the map
#define MAP_RES 	300 	// Resolution of the map (in mm) : size of the side of squares used to represent the maze
#define MAP_WIDTH 	15 		// Width of the map
#define MAP_HEIGHT 	7 		// Height of the map
#define END_COLOR 	NXT_COLOR_WHITE

// Cardinal points
#define NO_CARD 255  // No direction
#define NORTH 	0 	// North
#define EAST 	1 	// East
#define SOUTH 	2 	// South
#define WEST 	3 	// West
#define NO_EA 	4 	// North East
#define SO_EA 	5 	// South East
#define SO_WE 	6 	// South West
#define NO_WE 	7 	// North West

// Returns values for the map functions
#define UNKNOWN 0
#define NO_WALL 1
#define IS_WALL 2
#define ERROR   3
#define OUT_OF_MAP 255

//Defines the side of the square
#define CENTER_RES 20
#define MAPPING_RES 130


/* OSEK declarations */
DeclareEvent(EndOfMovement);
DeclareEvent(CellCenter);

DeclareCounter(SysTimerCnt);
DeclareTask(ColorReader);
DeclareTask(ColorMaintainer);
DeclareTask(DistanceReader);
DeclareTask(WheelsPositionReader);
DeclareTask(Localization);
DeclareTask(Mapping);
DeclareTask(Movement);
DeclareTask(PID_Control);
DeclareTask(MainController);

// About tasks
#define NB_TASKS 10
#define COLORREADER 	0
#define COLORMAINTAIN 	1
#define DISTREADER 		2
#define WPOSREADER 		3
#define LOCALIZATION 	4
#define MAPPING			5
#define MOVEMENT 		6
#define PIDCONTROL 		7
#define MAINCONTROL 	8
#define DISPLAY 	 	9

/* Function declarations */
U8 next_cp(U8 cp);
U8 previous_cp(U8 cp);
U8 is_cp(U8 val);
U8 coord_for_cp_square(U8 cp, int *x, int *y);
void display_cp(U8 cp);
double angle_to_reach(double xi, double yi, double xf, double yf);
void cell_center(int x, int y, double *realx, double *realy);
int between(S16 val, S16 expected, S8 limit);

void start_time(U8 taskID);
void stop_time(U8 taskID);
void display_scheduling();

#endif
