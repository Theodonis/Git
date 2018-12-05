/*
 * Application.h
 *
 *      Author: Erich Styger
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#define APP_CONFIG_PARSE_COMMAND_ENABLED (1)
#if APP_CONFIG_PARSE_COMMAND_ENABLED
#include "CLS1.h"
  uint8_t APP_ParseCommand(const unsigned char* cmd, bool *handled, const CLS1_StdIOType *io);
#endif

#define APP_SHELL_CONTROL (1)
#define APP_DEFAULT_HEATING_PWM  	(50)
#define APP_HEATING_TIME  			(1)
#define APP_DONTHEATING_TIME		(5)

  typedef enum {
    APP_HEATING_OFF,  /*!< Heater is on */
    APP_HEATING_ON  /*!< Heater is off */
  } APP_heating_state;

void APP_Run(void);

#endif /* APPLICATION_H_ */
