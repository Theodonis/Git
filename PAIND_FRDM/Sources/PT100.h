/*
 * PT100.h
 *
 *  Created on: 07.11.2018
 *      Author: Theo
 */

#ifndef SOURCES_PT100_H_
#define SOURCES_PT100_H_

#include "Platform.h"

#define PT100_CONFIG_100O				(0)
#define	PT100_CONFIG_1000O				(1)
#define	PT100_CONFIG_OP					(0)

PT100_Temp_t getTemp(uint16_t adc_value);


#endif /* SOURCES_PT100_H_ */
