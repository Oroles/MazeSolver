#ifndef _SHR_H
#define _SHR_H

#include "kernel.h"
#include "ecrobot_interface.h"

/* PORTS */
#define PORT_COLOR NXT_PORT_S4
#define PORT_DISTANCE_R NXT_PORT_S1
#define PORT_DISTANCE_F NXT_PORT_S2
#define PORT_DISTANCE_L NXT_PORT_S3
#define PORT_MOTOR_R NXT_PORT_B
#define PORT_MOTOR_L NXT_PORT_C

void set_color(U8 color);
void set_distanceL(S32 distanceL);
void set_distanceR(S32 distanceR);
void set_distanceF(S32 distanceF);
void set_wPositionL(int wpositionL);
void set_wPositionR(int wpositionR);

U8 get_color();
S32 get_distanceL();
S32 get_distanceR();
S32 get_distanceF();
int get_wPositionL();
int get_wPositionR();

#endif
