/*
 * AirSensor.h
 *
 *  Created on: 21.11.2018
 *      Author: Theo
 */

#ifndef SOURCES_SHT71_H_
#define SOURCES_SHT71_H_


#define SHT71_CONFIG_PARSE_COMMAND_ENABLED (0)

#if SHT71_CONFIG_PARSE_COMMAND_ENABLED
#include "CLS1.h"
  uint8_t SHT71_ParseCommand(const unsigned char* cmd, bool *handled, const CLS1_StdIOType *io);
#endif





void SHT71_Init(void);

#endif /* SOURCES_SHT71_H_ */
