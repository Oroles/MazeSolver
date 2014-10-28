/* helloworld.c for TOPPERS/ATK(OSEK) */ 
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include "utils/utils.h"
#include "utils/shared_variables.h"
#include "utils/display.h"
#include "slam/localization.h"
#include "slam/mapping.h"
#include "actions/movement.h"
#include "main_controller.h"

void ecrobot_device_initialize()
{
	ecrobot_init_nxtcolorsensor(PORT_COLOR, NXT_COLORSENSOR);
	ecrobot_init_sonar_sensor(PORT_DISTANCE_L);
	ecrobot_init_sonar_sensor(PORT_DISTANCE_F);
	ecrobot_init_sonar_sensor(PORT_DISTANCE_R);
	ecrobot_continuous_sonar_sensor(PORT_DISTANCE_F);
	ecrobot_off_sonar_sensor(PORT_DISTANCE_L);
	ecrobot_off_sonar_sensor(PORT_DISTANCE_R);
	init_mapping( 0x00 );
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

TASK(MainController) {
	while(1)
	{
		WaitEvent(NewDiscovery);
		ClearEvent(NewDiscovery);
		if(main_step()) { stop(); TerminateTask(); }
	}
	TerminateTask();
}

TASK(Display) {
	update_display();
	TerminateTask();
}

TASK(Localization) {
	update_localization();
	TerminateTask();
}

TASK(Mapping) {
	update_map();
	TerminateTask();
}

TASK(Movement) {
	do_movement();
	TerminateTask();
}

TASK(ColorReader) {
	ecrobot_process_bg_nxtcolorsensor();
	set_color(ecrobot_get_nxtcolorsensor_id(PORT_COLOR));
	TerminateTask();
}

TASK(DistanceReader) {
	static int measureF=TRUE;
	if(measureF) {
		set_distanceF(ecrobot_get_sonar_sensor(PORT_DISTANCE_F));

		ecrobot_off_sonar_sensor(PORT_DISTANCE_F);
		ecrobot_continuous_sonar_sensor(PORT_DISTANCE_L);
		ecrobot_continuous_sonar_sensor(PORT_DISTANCE_R);
		measureF=FALSE;
	}
	else {
		set_distanceL(ecrobot_get_sonar_sensor(PORT_DISTANCE_L));
		set_distanceR(ecrobot_get_sonar_sensor(PORT_DISTANCE_R));

		ecrobot_continuous_sonar_sensor(PORT_DISTANCE_F);
		ecrobot_off_sonar_sensor(PORT_DISTANCE_L);
		ecrobot_off_sonar_sensor(PORT_DISTANCE_R);
		measureF=TRUE;
	}
	TerminateTask();
}

TASK(WheelsPositionReader) {
	set_wPositionL(nxt_motor_get_count(PORT_MOTOR_L));
	set_wPositionR(nxt_motor_get_count(PORT_MOTOR_R));
	TerminateTask();
}

