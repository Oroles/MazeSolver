#ifndef _SHR_H
#define _SHR_H

#include "kernel.h"
#include "ecrobot_interface.h"

void set_rgb(S16* rgb);
void set_distanceL(S32 distanceL);
void set_distanceR(S32 distanceR);
void set_distanceF(S32 distanceF);
void set_wPositionL(int wpositionL);
void set_wPositionR(int wpositionR);

int is_stop_color();
S32 get_distanceL();
S32 get_distanceR();
S32 get_distanceF();
int get_wPositionL();
int get_wPositionR();

#endif
