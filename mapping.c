#include "kernel.h"
#include "ecrobot_interface.h"
#include "mapping.h"
#include "shared_variables.h"

U8 _map[15][7];
#define THRESHOLD_DISTANCE 10
#define NO_L_WALL 0x01
#define NO_R_WALL 0x02
#define NO_F_WALL 0x04
#define STOP_POSITION 0x08;

void update_map()
{
}

