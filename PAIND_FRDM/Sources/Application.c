/**
 * \file
 * \brief Main application file
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This provides the main application entry point.
 */

#include "Platform.h"
#include "Application.h"
#include "Event.h"
#include	"AD1.h"
#include	"PWM1.h"
/*#include "LED.h"
#include "WAIT1.h"
#include "CS1.h"
#include "KeyDebounce.h"
#include "CLS1.h"
#include "KIN1.h"
#include "SW6.h"
#include "SW7.h"	*/
#if PL_CONFIG_HAS_KEYS
  #include "Keys.h"
#endif
#if PL_CONFIG_HAS_SHELL
  #include "CLS1.h"
  #include "Shell.h"
  #include "RTT1.h"
#endif

#if PL_CONFIG_HAS_RTOS
  #include "FRTOS1.h"
  #include "RTOS.h"
#endif
#if PL_CONFIG_HAS_QUADRATURE
  #include "Q4CLeft.h"
  #include "Q4CRight.h"
#endif




#if PL_CONFIG_HAS_EVENTS

static void BtnMsg(int btn, const char *msg) {
#if PL_CONFIG_HAS_SHELL
  #if PL_CONFIG_HAS_SHELL_QUEUE
    uint8_t buf[48];

    UTIL1_strcpy(buf, sizeof(buf), "Button pressed: ");
    UTIL1_strcat(buf, sizeof(buf), msg);
    UTIL1_strcat(buf, sizeof(buf), ": ");
    UTIL1_strcatNum32s(buf, sizeof(buf), btn);
    UTIL1_strcat(buf, sizeof(buf), "\r\n");
    SHELL_SendString(buf);
  #else
    CLS1_SendStr("Button pressed: ", CLS1_GetStdio()->stdOut);
    CLS1_SendStr(msg, CLS1_GetStdio()->stdOut);
    CLS1_SendStr(": ", CLS1_GetStdio()->stdOut);
    CLS1_SendNum32s(btn, CLS1_GetStdio()->stdOut);
    CLS1_SendStr("\r\n", CLS1_GetStdio()->stdOut);
  #endif
#endif
}

void APP_EventHandler(EVNT_Handle event) {
	uint16_t i;
  /*! \todo handle events */
  switch(event) {
  case EVNT_STARTUP:
    {
      int i;
      for (i=0;i<5;i++) {
     }
    }
    break;
  case EVNT_LED_HEARTBEAT:
	  break;

  case EVNT_PWM_CHANGE:
	  (void)AD1_Measure(TRUE);  // measure all channel, wait for result
	  (void)AD1_GetChanValue16(VL_ADC_CHANEL_PWM_SET,&i);	   // Get AD conversion results
	  PWM1_SetDutyMS(i/650);	// Set PWM Heating to reed value
	  break;
  case EVNT_PT100_SENSOR1_READ:
	  (void)AD1_GetChanValue16(VL_ADC_CHANEL_PT100_1,&i);
	  break;
  case EVNT_PT100_SENSOR2_READ:
	  (void)AD1_GetChanValue16(VL_ADC_CHANEL_PT100_2,&i);
	  break;
  case EVNT_PT100_SENSOR3_READ:
	  (void)AD1_GetChanValue16(VL_ADC_CHANEL_PT100_3,&i);
	  break;
  case EVNT_PT100_SENSOR4_READ:
	  (void)AD1_GetChanValue16(VL_ADC_CHANEL_PT100_4,&i);
	  break;

#if PL_CONFIG_NOF_KEYS>=1
  case EVNT_SW1_PRESSED:
    BtnMsg(1, "pressed");
     break;
#endif
    default:
      break;
   } /* switch */
}
#endif /* PL_CONFIG_HAS_EVENTS */





void APP_Start(void) {

  PL_Init();
  __asm volatile("cpsie i"); /* enable interrupts */
  EVNT_SetEvent(EVNT_STARTUP);

  for(;;) {
	  EVNT_HandleEvent(APP_EventHandler,TRUE);
	//  WAIT1_Waitms(10);
  }
}


