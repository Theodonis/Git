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

#if PL_CONFIG_HAS_PWM_CHANGE

#else

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


void APP_init(void){
#if configSUPPORT_STATIC_ALLOCATION
  if (xTaskCreateStatic(application_task, "Application", APPLICATION_TASK_SIZE, NULL, tskIDLE_PRIORITY+4, &xStack[0], &xTaskTCBBuffer)==NULL) {
    for(;;){} /* task creation failed */
  }
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

