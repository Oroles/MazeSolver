/* helloworld.c for TOPPERS/ATK(OSEK) */ 
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include "utils/utils.h"
#include "utils/shared_variables.h"
#include "utils/display.h"
#include "slam/localization.h"
#include "slam/mapping.h"
#include "slam/path_finding.h"
#include "slam/node.h"
#include "actions/pid_control.h"
#include "actions/commands.h"
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
	U8 taskID=MAINCONTROL;

	WaitEvent(CellCenter);
	ClearEvent(CellCenter);
	init_localization();
	// The robot is now in the center of the cell, oriented in the EAST
	// Wait to have accurate values from Mapping
	stop_during_period(2000);
	WaitEvent(EndOfMovement);
	ClearEvent(EndOfMovement);
	main_step();
	while(1)
	{
		WaitEvent(CellCenter);
		ClearEvent(CellCenter);
		start_time(taskID);
		if(main_step()) {
			main_end();
			struct node* path = find_shortest_path(0,0,get_x(),get_y());
			print_path(path);
			free_list(path);
			TerminateTask();
		}
		stop_time(taskID);
	}
	TerminateTask();
}

TASK(Display) {
	U8 taskID=DISPLAY;
	start_time(taskID);

	update_display();

	stop_time(taskID);
	TerminateTask();
}

TASK(Localization) {
	U8 taskID=LOCALIZATION;
	start_time(taskID);

	update_localization();

	stop_time(taskID);
	TerminateTask();
}

TASK(Mapping) {
	U8 taskID=MAPPING;
	start_time(taskID);

	update_map();

	stop_time(taskID);
	TerminateTask();
}

TASK(PIDControl) {
	U8 taskID=PIDCONTROL;
	start_time(taskID);

	calculate_PID_output(get_mov_target_w());

	stop_time(taskID);
	TerminateTask();
}

TASK(Movement) {
	U8 taskID=MOVEMENT;
	start_time(taskID);

	do_movement();

	stop_time(taskID);
	TerminateTask();
}

TASK(ColorReader) {
	S16 rgb[3];
	ecrobot_get_nxtcolorsensor_rgb(PORT_COLOR,rgb);
	set_rgb(rgb);
	TerminateTask();
}

TASK(ColorMaintainer) {
	ecrobot_process_bg_nxtcolorsensor();
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

