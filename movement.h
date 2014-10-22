#ifndef _MOV_H
#define _MOV_H

#define THRESHOLD_DISTANCE  2
#define EXPECTED_VALUE 		7
#define PROPORTIONAL		3  
typedef enum { UP, RIGHT, DOWN, LEFT } orientation;

int KD = 10;
int KI = 1;
int KP = 10;
int error_int= 0;
int error_dev = 0;
double last_error = 0;

void stop();
void move_forward(U32 power);
void rotate_right(U32 power);
void rotate_left(U32 power);

orientation get_orientation();


#endif
