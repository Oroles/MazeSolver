#ifndef _SHR_H
#define _SHR_H

#include "kernel.h"
#include "ecrobot_interface.h"

void set_color(U8 color);
void set_distanceL(S32 distanceL);
void set_distanceR(S32 distanceR);
void set_distanceF(S32 distanceF);
void set_wpositionL(int wpositionL);
void set_wpositionR(int wpositionR);

U8 get_color();
S32 get_distanceL();
S32 get_distanceR();
S32 get_distanceF();
int get_wpositionL();
int get_wpositionR();

#endif
