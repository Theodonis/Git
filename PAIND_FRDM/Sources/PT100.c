/*
 * PT100.c
 *
 *  Created on: 07.11.2018
 *      Author: Theo
 */

#include "PT100.h"
#include "Platform.h"

#define ARRAY_SIZE (151)
#define ZEROPOINT_OFFSET (-70)

#if PT100_CONFIG_1000O
	uint16_t array_ADC[ARRAY_SIZE] = {4420,4443,4466,4489,4511,4534,4557,4580,4602,4625,4648,4670,4692,4715,4737,4760,4782,4804,4827,4849,4872,4894,4916,4939,4960,4983,5005,5027,5049,5071,5093,5116,5137,5160,5181,5204,5225,5247,5269,5291,5313,5335,5356,5378,5400,5422,5444,5465,5487,5509,5530,5551,5573,5595,5616,5638,5659,5681,5702,5723,5745,5766,5788,5809,5830,5851,5873,5894,5915,5937,5958,5979,6000,6021,6042,6063,6084,6105,6126,6147,6168,6189,6210,6231,6252,6273,6294,6315,6336,6356,6377,6397,6418,6439,6460,6480,6501,6522,6542,6563,6583,6604,6624,6645,6665,6686,6706,6727,6747,6767,6788,6808,6828,6849,6869,6889,6910,6930,6950,6970,6990,7010,7030,7050,7071,7090,7111,7131,7150,7171,7190,7211,7230,7250,7270,7290,7310,7330,7349,7369,7389,7409,7428,7448,7468,7488,7507,7527,7546,7566,7586};
#endif
#if PT100_CONFIG_100O
	uint16_t array_ADC[ARRAY_SIZE];
#endif
#if PT100_CONFIG_OP
	;
#endif



PT100_Temp_t getTemp(uint16_t adc_value){
	PT100_Temp_t temp;
	int8_t i=0;
	while(adc_value>(array_ADC[i])){
		i++;
	}
	temp.degree = ZEROPOINT_OFFSET+i-1;
	temp.dec	= 0;
	i=array_ADC[i]-adc_value;
	for(uint8_t k=0;k<i;k+=2){
		temp.dec++;
	}

	return temp;
}


