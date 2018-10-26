/**
 * \file
 * \brief Common platform configuration module.
 * \author Erich Styger, erich.styger@hslu.ch
 */

#include "Platform.h"
#if PL_CONFIG_HAS_LEDS
  #include "LED.h"
#endif
#if PL_CONFIG_HAS_EVENTS
  #include "Event.h"
#endif
#if PL_CONFIG_HAS_TIMER
  #include "Timer.h"
#endif

#if PL_CONFIG_HAS_TRIGGER
  #include "Trigger.h"
#endif
#if PL_CONFIG_HAS_DEBOUNCE
  #include "Debounce.h"
  #include "KeyDebounce.h"
#endif
#if PL_CONFIG_HAS_RTOS
  #include "RTOS.h"
#endif
#if PL_CONFIG_HAS_SHELL
  #include "Shell.h"
#endif
#if PL_CONFIG_HAS_SHELL_QUEUE
  #include "ShellQueue.h"
#endif
#if PL_CONFIG_HAS_SEMAPHORE
  #include "Sem.h"
#endif


void PL_Init(void) {
#if PL_CONFIG_HAS_LEDS
  LED_Init();
#endif
#if PL_CONFIG_HAS_EVENTS
  EVNT_Init();
#endif
#if PL_CONFIG_HAS_TIMER
  TMR_Init();
#endif
#if PL_CONFIG_HAS_KEYS
  KEY_Init();
#endif
#if PL_CONFIG_HAS_TRIGGER
  TRG_Init();
#endif
#if PL_CONFIG_HAS_DEBOUNCE
  KEYDBNC_Init();
#endif
#if PL_CONFIG_HAS_RTOS
  RTOS_Init();
#endif
#if PL_CONFIG_HAS_SHELL
  SHELL_Init();
#endif
#if PL_CONFIG_HAS_SHELL_QUEUE
  SQUEUE_Init();
#endif
#if PL_CONFIG_HAS_SEMAPHORE
  SEM_Init();
#endif

}

void PL_Deinit(void) {

#if PL_CONFIG_HAS_SEMAPHORE
  SEM_Deinit();
#endif
#if PL_CONFIG_HAS_SHELL_QUEUE
  SQUEUE_Deinit();
#endif
#if PL_CONFIG_HAS_RTOS
  RTOS_Deinit();
#endif
#if PL_CONFIG_HAS_DEBOUNCE
  KEYDBNC_Deinit();
#endif
#if PL_CONFIG_HAS_TRIGGER
  TRG_Deinit();
#endif
#if PL_CONFIG_HAS_KEYS
  KEY_Deinit();
#endif
#if PL_CONFIG_HAS_TIMER
  TMR_Deinit();
#endif
#if PL_CONFIG_HAS_EVENTS
  EVNT_Init();
#endif
#if PL_CONFIG_HAS_LEDS
  LED_Deinit();
#endif
}
