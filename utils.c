#include "utils.h"

int next_cp(int cp) { return cp+=2; }

int is_cp(int val) { return (val==NORTH)||(val==EAST)||(val==SOUTH)||(val==WEST); }
