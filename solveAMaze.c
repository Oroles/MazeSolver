/* helloworld.c for TOPPERS/ATK(OSEK) */ 
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include "shared_variables.h"
#include "localization.h"
#include "mapping.h"

/* PORTS */
#define PORT_COLOR NXT_PORT_S4
#define PORT_DISTANCE_R NXT_PORT_S1
#define PORT_DISTANCE_F NXT_PORT_S2
#define PORT_DISTANCE_L NXT_PORT_S3
#define PORT_MOTOR_R NXT_PORT_B
#define PORT_MOTOR_L NXT_PORT_C

/* OSEK declarations */

DeclareCounter(SysTimerCnt);
DeclareTask(ColorReader);
DeclareTask(DistanceReader);
DeclareTask(WheelsPositionReader);
DeclareTask(Localization);
DeclareTask(Mapping);

void ecrobot_device_initialize()
{
	ecrobot_init_nxtcolorsensor(PORT_COLOR, NXT_COLORSENSOR);
	ecrobot_init_sonar_sensor(PORT_DISTANCE_L);
	ecrobot_init_sonar_sensor(PORT_DISTANCE_F);
	ecrobot_init_sonar_sensor(PORT_DISTANCE_R);
	init_map();
}
void ecrobot_device_terminate()
{
	ecrobot_term_nxtcolorsensor(PORT_COLOR);
	ecrobot_term_sonar_sensor(PORT_DISTANCE_L);
	ecrobot_term_sonar_sensor(PORT_DISTANCE_F);
	ecrobot_term_sonar_sensor(PORT_DISTANCE_R);
}

/* nxtOSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void){
  StatusType ercd;
  ercd = SignalCounter(SysTimerCnt); /* Increment OSEK Alarm Counter */
  if (ercd != E_OK)
	{
	  ShutdownOS(ercd);
	}
}

void displayData()
{
	display_clear(1);
	display_goto_xy(0,0);display_string("LD");display_goto_xy(2,0);display_int(get_distanceL(),3);
	display_goto_xy(0,1);display_string("FD");display_goto_xy(2,1);display_int(get_distanceF(),3);
	display_goto_xy(0,2);display_string("RD");display_goto_xy(2,2);display_int(get_distanceR(),3);
	display_goto_xy(0,3);display_string("WL");display_goto_xy(2,3);display_int(get_wPositionL(),4);
	display_goto_xy(0,4);display_string("WR");display_goto_xy(2,4);display_int(get_wPositionR(),4);
	display_goto_xy(0,5);display_string("PX");display_goto_xy(2,5);display_int(get_x(),3);
	display_goto_xy(0,6);display_string("PY");display_goto_xy(2,6);display_int(get_y(),3);
	
	display_update();
}

TASK(Localization) {
	update_localization();
	displayData();
	TerminateTask();
}

TASK(Mapping) {
	update_map();
	TerminateTask();
}

TASK(ColorReader) {
	ecrobot_process_bg_nxtcolorsensor();
	set_color(ecrobot_get_nxtcolorsensor_id(PORT_COLOR));
	TerminateTask();
}

TASK(DistanceReader) {
	set_distanceL(ecrobot_get_sonar_sensor(PORT_DISTANCE_L));
	set_distanceF(ecrobot_get_sonar_sensor(PORT_DISTANCE_F));
	set_distanceR(ecrobot_get_sonar_sensor(PORT_DISTANCE_R));
	TerminateTask();
}

TASK(WheelsPositionReader) {
	set_wPositionL(nxt_motor_get_count(PORT_MOTOR_L));
	set_wPositionR(nxt_motor_get_count(PORT_MOTOR_R));
	TerminateTask();
}

