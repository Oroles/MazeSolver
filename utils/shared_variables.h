#ifndef _SHR_H
#define _SHR_H

#include "kernel.h"
#include "ecrobot_interface.h"

void set_color(U8 color);
void set_distanceL(S32 distanceL);
void set_distanceR(S32 distanceR);
void set_distanceF(S32 distanceF);
void set_wPositionL(int wpositionL);
void set_wPositionR(int wpositionR);

U8 get_color();
U8 get_color_counter();
S32 get_distanceL();
S32 get_distanceR();
S32 get_distanceF();
int get_wPositionL();
int get_wPositionR();

#endif
