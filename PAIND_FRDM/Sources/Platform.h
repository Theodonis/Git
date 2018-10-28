/**
 * \file
 * \brief Common platform configuration interface.
 * \author Erich Styger, erich.styger@hslu.ch
 */

#ifndef SOURCES_INTRO_COMMON_PLATFORM_H_
#define SOURCES_INTRO_COMMON_PLATFORM_H_

/* include shared header files */
#include "PE_Types.h" /* common Processor Expert types: bool, NULL, ... */
#include "Cpu.h" /* for PEcfg_RoboV2/PEcfg_RoboV1 */

#define PL_CONFIG_HAS_LEDS			(0)
#define PL_CONFIG_HAS_EVENTS		(1)
#define	PL_CONFIG_HAS_TIMER			(1)
#define	PL_CONFIG_HAS_TRIGGER		(0)
#define PL_CONFIG_HAS_DEBOUNCE		(0)
#define PL_CONFIG_HAS_RTOS			(0)
#define PL_CONFIG_HAS_SHELL			(0)
#define	PL_CONFIG_HAS_SHELL_QUEUE	(0)
#define PL_CONFIG_HAS_SEMAPHORE		(0)

#define	VL_ADC_CHANEL_PWM_SET		(0)
#define	VL_ADC_CHANEL_PT100_1		(1)
#define	VL_ADC_CHANEL_PT100_2		(2)
#define	VL_ADC_CHANEL_PT100_3		(3)
#define	VL_ADC_CHANEL_PT100_4		(4)

 /* \brief Driver de-initialization
 */
void PL_Deinit(void);

/*!
 * \brief  driver initialization
 */
void PL_Init(void);

#endif /* SOURCES_INTRO_COMMON_PLATFORM_H_ */
