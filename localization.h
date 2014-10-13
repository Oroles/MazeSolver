#ifndef _LOC_H
#define _LOC_H

#include "kernel.h"
#include "ecrobot_interface.h"

DeclareResource(UpdateLocker);

int get_x();
int get_y();
int get_d();
void update_localization();

#endif
