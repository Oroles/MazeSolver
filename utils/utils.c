#include <stdlib.h>
#include <math.h>
#include "ecrobot_interface.h"
#include "utils.h"

U8 next_cp(U8 cp) {
	switch(cp) {
		case NORTH: case NO_EA: return EAST;
		case EAST: case SO_EA: return SOUTH;
		case SOUTH: case SO_WE: return WEST;
		case WEST: case NO_WE: return NORTH;
		default: return -1;
	}
}

U8 previous_cp(U8 cp) {
	switch(cp) {
		case NORTH: case NO_EA: return WEST;
		case EAST: case SO_EA: return NORTH;
		case SOUTH: case SO_WE: return EAST;
		case WEST: case NO_WE: return SOUTH;
		default: return -1;
	}
}

U8 is_cp(U8 val) { return (val==NORTH)||(val==EAST)||(val==SOUTH)||(val==WEST); }

U8 coord_for_cp_square(U8 cp, int *x, int *y) {
	switch(cp) {
		case NORTH: (*y)++; return TRUE;
		case EAST: (*x)++; return TRUE;
		case SOUTH: (*y)--; return TRUE;
		case WEST: (*x)--; return TRUE;
		default: return FALSE;
	}
}

void display_cp(U8 cp) {
	switch(cp) {
		case NORTH: display_string("NORTH"); break;
		case EAST: display_string("EAST"); break;
		case SOUTH: display_string("EAST"); break;
		case WEST: display_string("WEST"); break;
		default: display_string("????"); break;
	}
}

double angle_to_reach(double xi, double yi, double xf, double yf) {
	double opp=yf-yi,adj=xf-xi;
	double res;
	if(adj!=0 && opp!=0) {
		res=fabs(opp/adj);
		res=atan(res)/RAD;
	}
	else if(opp!=0) res=90;
	else res=0;

	if(opp>=0) {
		if(adj>=0) return res;
		else return 180-res;
	}
	else {
		if(adj>=0) return 360-res;
		else return 180+res;
	}
}

void cell_center(int x, int y, double *realx, double *realy) {
	*realx=(x+0.5)*MAP_RES;
	*realy=(y+0.5)*MAP_RES;
}

int between(S16 val, S16 expected, S8 limit) {
	if( ( val > expected - 20 ) &&
		( val < expected + 20 ) ) {
		return 1;
	}
	return 0;
}

#define TMSIZE 49
U8 ring_timeline[TMSIZE];
U8 ring_tm_index=0;

U32 worst_case_exe_time[NB_TASKS]={0};
U32 st_time[NB_TASKS]={0};
int preemption=0;
int max_preemption=0;

void display_timeline() {
	int x=0,y=0,i=ring_tm_index,init=ring_tm_index;
	display_clear(0);
	do {
		display_goto_xy(x,y);
		display_int(ring_timeline[i]-NB_TASKS,2);
		i=(i+1)%TMSIZE;
		x=(x+2)%14;
		if(x==0) y=(y+1);
	}while(i!=init);
	display_update();
}

void start_time(U8 taskID) {
	preemption++;
	st_time[taskID]=systick_get_ms();
	ring_timeline[ring_tm_index]=NB_TASKS+taskID;
	ring_tm_index= (ring_tm_index+1) % TMSIZE;
}

void stop_time(U8 taskID) {
	U32 exe_time=systick_get_ms()-st_time[taskID];
	if(exe_time>worst_case_exe_time[taskID]) worst_case_exe_time[taskID]=exe_time;
	preemption--;
	if(preemption>max_preemption) max_preemption=preemption;
	ring_timeline[ring_tm_index]=NB_TASKS-taskID;
	ring_tm_index= (ring_tm_index+1) % TMSIZE;
}

void display_scheduling() {
	display_clear(0);
	display_goto_xy(0,0);display_string("LOC");display_goto_xy(5,0);display_int(worst_case_exe_time[LOCALIZATION],3);
	display_goto_xy(0,1);display_string("MAP");display_goto_xy(5,1);display_int(worst_case_exe_time[MAPPING],3);
	display_goto_xy(0,2);display_string("MAIN");display_goto_xy(5,2);display_int(worst_case_exe_time[MAINCONTROL],3);
	display_goto_xy(0,3);display_string("MOVE");display_goto_xy(5,3);display_int(worst_case_exe_time[MOVEMENT],3);
	display_goto_xy(0,4);display_string("PID");display_goto_xy(5,4);display_int(worst_case_exe_time[PIDCONTROL],3);
	display_goto_xy(0,5);display_string("DISP");display_goto_xy(5,5);display_int(worst_case_exe_time[DISPLAY],3);
	//display_goto_xy(0,4);display_string("PID");display_goto_xy(5,6);display_int(worst_case_exe_time[MAINCONTROL],3);
	display_goto_xy(0,7);display_string("MAXPREEMPT");display_goto_xy(11,7);display_int(max_preemption,3);
	display_update();
}
