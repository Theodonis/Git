/*
 * Sensor.h
 *
 *  Created on: 14.10.2018
 *      Author: Erich Styger
 */

#ifndef SOURCES_SENSOR_H_
#define SOURCES_SENSOR_H_

#define SENS_CONFIG_PARSE_COMMAND_ENABLED (1)
#if SENS_CONFIG_PARSE_COMMAND_ENABLED
#include "CLS1.h"
  uint8_t SENSOR_ParseCommand(const unsigned char* cmd, bool *handled, const CLS1_StdIOType *io);
#endif

float SENSOR_GetTemperature(void);
float SENSOR_GetHumidity(void);

void SENSOR_Init(void);


#endif /* SOURCES_SENSOR_H_ */
