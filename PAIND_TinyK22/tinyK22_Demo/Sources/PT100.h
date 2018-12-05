/*
 * PT100.h
 *
 *  Created on: 18.11.2018
 *      Author: Theo
 */

#ifndef SOURCES_PT100_H_
#define SOURCES_PT100_H_

#include "Platform.h"
#include "UTIL1.h"

#define ARRAY_SIZE (151)
#define ZEROPOINT_OFFSET (-70)

// Config which way the PT100 is used
#define PT100_CONFIG_1000O		(1)	//1k Ohm preresistor and 3.3V
#define PT100_CONFIG_100O		(0) //100 Ohm preresistor and 3.3V
#define PT100_CONFIG_1mA		(0)	// constant current 1mA

typedef struct PT100_Temp {
	int8_t degree; 		//Temp in °C
	int8_t dec;			// Temp in °C < 1
}PT100_Temp_t;

PT100_Temp_t getTemp(uint16_t adc_value);




#endif /* SOURCES_PT100_H_ */
