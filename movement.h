#ifndef _MOV_H
#define _MOV_H

#include "kernel.h"
#include "ecrobot_interface.h"

void stop();
void move_forward(U32 power);
void rotate_right();
void rotate_left();

#endif