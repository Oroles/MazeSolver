#ifndef _MOV_H
#define _MOV_H

#define THRESHOLD_DISTANCE 2
typedef enum { UP, RIGHT, DOWN, LEFT } orientation;

void stop();
void move_forward(U32 power);
void rotate_right(U32 power);
void rotate_left(U32 power);

orientation get_orientation();


#endif
