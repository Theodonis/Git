/*
 * Application.c
 *      Author: Erich Styger
 */
#include "Platform.h"
#include "Application.h"
#include "LED1.h"
#include "FAT1.h"
#include "CLS1.h"
#include "WAIT1.h"
#include "UTIL1.h"
#include "FRTOS1.h"
#include "Shell.h"
#include "AD1.h"
#include "PWM1.h"
#include "SDCard.h"
#include "DataLogger.h"
#include "SwTimer.h"
#include "Sensor.h"

#if PL_USE_HW_RTC
  #include "RTC1.h"
#endif



#if configSUPPORT_DYNAMIC_ALLOCATION && configAPPLICATION_ALLOCATED_HEAP
  #if configUSE_HEAP_SECTION_NAME
    uint8_t __attribute__((section (configHEAP_SECTION_NAME_STRING)))  ucHeap[configTOTAL_HEAP_SIZE]; /* custom heap allocation */
  #else
    uint8_t ucHeap[configTOTAL_HEAP_SIZE]; /* custom heap allocation */
  #endif
#endif



#define APPLICATION_TASK_SIZE   configMINIMAL_STACK_SIZE+100
#if configSUPPORT_STATIC_ALLOCATION
  #if configUSE_HEAP_SECTION_NAME
    #define SECTION __attribute__((section (configHEAP_SECTION_NAME_STRING)))
  #else
    #define SECTION /* empty */
  #endif
  static StaticTask_t SECTION xTaskTCBBuffer;
  static StackType_t SECTION xStack[APPLICATION_TASK_SIZE];
#endif

#if APP_SHELL_CONTROL
  uint8_t heatintime_s = APP_HEATING_TIME;
  uint8_t dontheatintime_s = APP_DONTHEATING_TIME;
  uint16_t dutyPWM = APP_DEFAULT_HEATING_PWM;
  APP_heating_state heating_state = APP_HEATING_OFF;
  static SemaphoreHandle_t appsem;

#endif

#if APP_SHELL_CONTROL

APP_heating_state APP_getHeatingState(void){
	APP_heating_state val;

  	(void)xSemaphoreTakeRecursive(appsem, portMAX_DELAY);
  	val = heating_state;
  	(void)xSemaphoreGiveRecursive(appsem);
  	return val;
}

void APP_HeaterOn(void){
  	(void)xSemaphoreTakeRecursive(appsem, portMAX_DELAY);
  	heating_state = APP_HEATING_ON;
  	(void)xSemaphoreGiveRecursive(appsem);
}
void APP_HeaterOff(void){
  	(void)xSemaphoreTakeRecursive(appsem, portMAX_DELAY);
  	heating_state = APP_HEATING_OFF;
  	(void)xSemaphoreGiveRecursive(appsem);
}


uint8_t APP_getHeatingTime(void){
	uint8_t val;

	(void)xSemaphoreTakeRecursive(appsem, portMAX_DELAY);
	val = heatintime_s;
	(void)xSemaphoreGiveRecursive(appsem);
	return val;
}
void APP_setHeatingTime(uint8_t time){

	(void)xSemaphoreTakeRecursive(appsem, portMAX_DELAY);
	heatintime_s = time;
	(void)xSemaphoreGiveRecursive(appsem);
}

uint8_t APP_getDontHeatingTime(void){
	uint8_t val;
	(void)xSemaphoreTakeRecursive(appsem, portMAX_DELAY);
	val = dontheatintime_s;
	(void)xSemaphoreGiveRecursive(appsem);

	return val;
}
void APP_setDontHeatingTime(uint8_t time){
	(void)xSemaphoreTakeRecursive(appsem, portMAX_DELAY);
	dontheatintime_s = time;
	(void)xSemaphoreGiveRecursive(appsem);
}

uint16_t APP_getDutyPWM(void){
	uint16_t val;

	(void)xSemaphoreTakeRecursive(appsem, portMAX_DELAY);
	val = dutyPWM;
	(void)xSemaphoreGiveRecursive(appsem);
	return val;
}
void APP_setDutyPWM(uint16_t duty){
	(void)xSemaphoreTakeRecursive(appsem, portMAX_DELAY);
	dutyPWM = duty;
	(void)xSemaphoreGiveRecursive(appsem);
}



#endif
static void application_task(void* pvParameter){
	uint16_t i;

	for(;;){
		#if PL_CONFIG_HAS_PWM_CHANGE
		 (void)AD1_Measure(TRUE);  									// measure all channel, wait for result
		 (void)AD1_GetChanValue16(0,&i);	  	// Get AD conversion results
		#else
		 i=APP_DEFAULT_HEATING_PWM;
		#endif


		 PWM1_SetDutyUS(i/6500);									// Set PWM Duty for Heating to measured value in
		// SendString((unsigned char*)"PWM:\t", &deviceData);		// hear should be written to a queue, from which the Logger task can read
		// printUInt16(i/6500);
		 vTaskDelay(pdMS_TO_TICKS(APP_HEATING_TIME));
		 PWM1_SetDutyUS(APP_DEFAULT_HEATING_PWM);
		 vTaskDelay(pdMS_TO_TICKS(APP_DONTHEATING_TIME));
	}
}

#if APP_CONFIG_PARSE_COMMAND_ENABLED
static uint8_t PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[64];


  CLS1_SendStatusStr((unsigned char*)"APP", (unsigned char*)"\r\n", io->stdOut);
  UTIL1_Num8uToStr(buf, sizeof(buf),APP_getHeatingTime());
  UTIL1_strcat(buf, sizeof(buf), " s\r\n");
  CLS1_SendStatusStr((unsigned char*)"  On time", buf, io->stdOut);

  UTIL1_Num8uToStr(buf, sizeof(buf),APP_getDontHeatingTime());
  UTIL1_strcat(buf, sizeof(buf), " s\r\n");
  CLS1_SendStatusStr((unsigned char*)"  Off time", buf, io->stdOut);

  UTIL1_Num16uToStr(buf, sizeof(buf),APP_getDutyPWM());
  UTIL1_strcat(buf, sizeof(buf), " ‰\r\n\n");
  CLS1_SendStatusStr((unsigned char*)"  PWMduty", buf, io->stdOut);

  buf[0] = '\0';
  UTIL1_strcat(buf,sizeof(buf),(unsigned char*)(APP_getHeatingState()==APP_HEATING_OFF?"OFF":"ON"));
  CLS1_SendStatusStr((unsigned char*)"  Heater is", buf, io->stdOut);
  CLS1_SendStr((unsigned char*)"\r\n", io->stdOut);

  return ERR_OK;
}

static uint8_t PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"APP", (unsigned char*)"Group of APP commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Print help or status information\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  set PWM <number>", (unsigned char*)"Set the PWM Duty from 0-1000\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  set on|off time <number>", (unsigned char*)"set the time for heating or not periode in s\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  set heater on|off", (unsigned char*)"Start or stop heater\r\n", io->stdOut);
  return ERR_OK;
}
#endif /* APP_CONFIG_PARSE_COMMAND_ENABLED */

#if APP_CONFIG_PARSE_COMMAND_ENABLED
uint8_t  APP_ParseCommand(const unsigned char* cmd, bool *handled, const CLS1_StdIOType *io) {
  uint8_t res = ERR_OK;
  int32_t val;
  const unsigned char *p;

  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP) == 0
    || UTIL1_strcmp((char*)cmd, "APP help") == 0)
  {
    *handled = TRUE;
    return PrintHelp(io);
  } else if (   (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0)
             || (UTIL1_strcmp((char*)cmd, "APP status")==0)
            )
  {
    *handled = TRUE;
    res = PrintStatus(io);
  } else if (   UTIL1_strcmp((char*)cmd, "APP set heater off")==0)
      {
  	 APP_HeaterOff();
  	 *handled = TRUE;
  	 res = PrintStatus(io);
  } else if (   UTIL1_strcmp((char*)cmd, "APP set heater on")==0)
    {
	 APP_HeaterOn();
	 *handled = TRUE;
	 res = PrintStatus(io);
  } else if (UTIL1_strncmp((char*)cmd, (char*)"APP set on time ", sizeof("APP set on time ")-1)==0) {
	 p = cmd+sizeof("APP set on time");
	 if (UTIL1_xatoi(&p, &val)==ERR_OK && val >=0 && val<= 60) {
		 APP_setHeatingTime(val);
	   *handled = TRUE;
	   res = PrintStatus(io);
	 } else {
	   CLS1_SendStr((unsigned char*)"Wrong argument, must be in the range 0...60\r\n", io->stdErr);
	   res = ERR_FAILED;
	 }
  } else if (UTIL1_strncmp((char*)cmd, (char*)"APP set off time ", sizeof("APP set off time ")-1)==0) {
		 p = cmd+sizeof("APP set off time");
		 if (UTIL1_xatoi(&p, &val)==ERR_OK && val >=0 && val<=60) {
			 APP_setDontHeatingTime(val);
		   *handled = TRUE;
		   res = PrintStatus(io);
		 } else {
		   CLS1_SendStr((unsigned char*)"Wrong argument, must be in the range 0...60\r\n", io->stdErr);
		   res = ERR_FAILED;
		 }
  }else if (UTIL1_strncmp((char*)cmd, (char*)"APP set PWM ", sizeof("APP set PWM ")-1)==0) {
		 p = cmd+sizeof("APP set PWM");
		 if (UTIL1_xatoi(&p, &val)==ERR_OK && val >=0 && val<=1000) {
			 APP_setDutyPWM(val);
		   *handled = TRUE;
		   res = PrintStatus(io);
		 } else {
		   CLS1_SendStr((unsigned char*)"Wrong argument, must be in the range 0...1000\r\n", io->stdErr);
		   res = ERR_FAILED;
		 }
	  }
  return res;
}
#endif /* APP_CONFIG_PARSE_COMMAND_ENABLED */



void APP_init(void){
#if configSUPPORT_STATIC_ALLOCATION
  if (xTaskCreateStatic(application_task, "Application", APPLICATION_TASK_SIZE, NULL, tskIDLE_PRIORITY+4, &xStack[0], &xTaskTCBBuffer)==NULL) {
    for(;;){} /* task creation failed */
  }
#if APP_SHELL_CONTROL
  appsem = xSemaphoreCreateRecursiveMutex();
  if (appsem==NULL) { /* creation failed? */
	  for(;;);
  }
  vQueueAddToRegistry(appsem, "appsem");
#endif
#elif configSUPPORT_DYNAMIC_ALLOCATION
  if (xTaskCreate(application_task, "Application",APPLICATION_TASK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS) {
    for(;;){} /* error! probably out of memory */
  }
#else
  #error "Either configSUPPORT_STATIC_ALLOCATION or configSUPPORT_DYNAMIC_ALLOCATION has to be enabled"
#endif
}

void APP_Run(void) {
//  DiskTest();
  SHELL_Init();
  SWT_Init();


#if configUSE_TRACE_HOOKS /* using Percepio FreeRTOS+Trace */
  if (uiTraceStart()==0) {
    for(;;){} /* error starting trace recorder. Not setup for enough queues/tasks/etc? */
  }
#endif
#if PL_CONFIG_HAS_LOGGER
 LOGGER_Init();
#endif
  SENSOR_Init();
  APP_init();
  vTaskStartScheduler();
}

