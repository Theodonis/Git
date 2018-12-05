/*
 * Sensor.c
 *
 *  Created on: 14.10.2018
 *      Author: Erich Styger
 */

#include "Platform.h"
#include "Sensor.h"
#include "FreeRTOS.h"
#include "task.h"
#if PL_CONFIG_HAS_SHT71
  #include "SHT71.h"
#endif
#if PL_CONFIG_HAS_TSL2561
  #include "TSL1.h"
#endif
#if PL_CONFIG_HAS_SGP30
  #include "SGP30.h"
#endif
#if PL_CONFIG_HAS_SGP30
  #include "SGP30.h"
#endif
#if PL_CONFIG_HAS_MMA8451
  #include "MMA1.h"
#endif
#if PL_CONFIG_HAS_AMG8833
  #include "AMG8833.h"
#endif
#include "TmDt1.h"

#if PL_CONFIG_HAS_SHT71
static float temperature, humidity;
static SemaphoreHandle_t sensorsem;

float SENSOR_GetTemperature(void) {
  float val;

  (void)xSemaphoreTakeRecursive(sensorsem, portMAX_DELAY);
  val = temperature;
  (void)xSemaphoreGiveRecursive(sensorsem);
  return val;
}

float SENSOR_GetHumidity(void) {
  float val;

  (void)xSemaphoreTakeRecursive(sensorsem, portMAX_DELAY);
  val = humidity;
  (void)xSemaphoreGiveRecursive(sensorsem);
  return val;
}

static void SENSOR_SetHumidity(float h) {
  (void)xSemaphoreTakeRecursive(sensorsem, portMAX_DELAY);
  humidity = h;
  (void)xSemaphoreGiveRecursive(sensorsem);
}

static void SENSOR_SetTemperature(float t) {
  (void)xSemaphoreTakeRecursive(sensorsem, portMAX_DELAY);
  temperature = t;
  (void)xSemaphoreGiveRecursive(sensorsem);
}

static void SHT71Task(void *pv) {
  float temp, hum;

  vTaskDelay(pdMS_TO_TICKS(12)); /* give sensors time to power up */

  CLS1_SendStr((uint8_t*)"Initializing SHT71.\r\n", CLS1_GetStdio()->stdOut);
  SHT71_Init();
  vTaskDelay(pdMS_TO_TICKS(12));
  for(;;) {
    if (SHT71_ReadTempHum(&temp, &hum)==ERR_OK) {
      SENSOR_SetTemperature(temp);
      SENSOR_SetHumidity(hum);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#endif

#if SENSOR_CONFIG_PARSE_COMMAND_ENABLED
static uint8_t PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[32];


  CLS1_SendStatusStr((unsigned char*)"Sensor", (unsigned char*)"\r\n", io->stdOut);

  UTIL1_NumFloatToStr(buf, sizeof(buf), SENSOR_GetTemperature(), 2);
  UTIL1_strcat(buf, sizeof(buf), "°C\r\n");
  CLS1_SendStatusStr((unsigned char*)"  temp", buf, io->stdOut);

  UTIL1_NumFloatToStr(buf, sizeof(buf), SENSOR_GetHumidity(), 2);
  UTIL1_strcat(buf, sizeof(buf), " RH\r\n");
  CLS1_SendStatusStr((unsigned char*)"  humidity", buf, io->stdOut);

  return ERR_OK;
}

static uint8_t PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"Sensor", (unsigned char*)"Group of Sensor commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Print help or status information\r\n", io->stdOut);
  return ERR_OK;
}
#endif /* SENSOR_CONFIG_PARSE_COMMAND_ENABLED */

#if SENSOR_CONFIG_PARSE_COMMAND_ENABLED
uint8_t SENSOR_ParseCommand(const unsigned char* cmd, bool *handled, const CLS1_StdIOType *io) {
  uint8_t res = ERR_OK;

  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP) == 0
    || UTIL1_strcmp((char*)cmd, "Sensor help") == 0)
  {
    *handled = TRUE;
    return PrintHelp(io);
  } else if (   (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0)
             || (UTIL1_strcmp((char*)cmd, "Sensor status")==0)
            )
  {
    *handled = TRUE;
    res = PrintStatus(io);
  }
  return res;
}
#endif /* SENSOR_CONFIG_PARSE_COMMAND_ENABLED */

#if PL_CONFIG_HAS_AMG8833
static float AMGBuf[AMG88xx_PIXEL_ARRAY_SIZE];
#endif

static void SensorTask(void *pv) {
#if PL_CONFIG_HAS_MMA8451 || PL_CONFIG_HAS_TSL2561
  uint8_t res;
#endif

  vTaskDelay(pdMS_TO_TICKS(500)); /* give sensors time to power up */
#if 0 && PL_CONFIG_HAS_AMG8833
  if (AMG_Init()!=ERR_OK) {
    CLS1_SendStr((uint8_t*)"Failed initializing AMG8833!\r\n", CLS1_GetStdio()->stdErr);
  }
#endif
#if PL_CONFIG_HAS_RTC_DS3231
  CLS1_SendStr((uint8_t*)"Enabling Time and Date.\r\n", CLS1_GetStdio()->stdOut);
  TmDt1_Init(); /* get time/date from external RTC */
#endif
#if PL_CONFIG_HAS_TSL2561
  uint16_t tvoc, co2;
  uint16_t TVOC_base, eCO2_base;
  int cntr = 0;

  CLS1_SendStr((uint8_t*)"Enabling TLS2561 sensor.\r\n", CLS1_GetStdio()->stdOut);
  TSL1_Init();

  res = TSL1_Disable();
  vTaskDelay(pdMS_TO_TICKS(5));
  if (res!=ERR_OK) {
    for(;;){}
  }
  vTaskDelay(pdMS_TO_TICKS(50));
  res = TSL1_Enable();
  vTaskDelay(pdMS_TO_TICKS(5));
  if (res!=ERR_OK) {
    for(;;){}
  }

  res = TSL1_SetIntegrationTime(TSL2561_INTEGRATION_TIME_13MS);
  vTaskDelay(pdMS_TO_TICKS(5));
  if (res!=ERR_OK) {
    for(;;){}
  }
  res = TSL1_SetGain(TSL2561_GAIN_16X);
  if (res!=ERR_OK) {
    for(;;){}
  }
#endif
#if PL_CONFIG_HAS_SGP30
   CLS1_SendStr((uint8_t*)"Enabling SGP30 sensor.\r\n", CLS1_GetStdio()->stdOut);
   SGP30_Init();
#endif
#if PL_CONFIG_HAS_MMA8451
   bool isEnabled = FALSE;

   res = MMA1_isEnabled(&isEnabled);
   if (res!=ERR_OK) {
     CLS1_SendStr((uint8_t*)"ERROR: Cannot access MMA8541!\r\n", CLS1_GetStdio()->stdErr);
   } else if (!isEnabled) {
     CLS1_SendStr((uint8_t*)"Enabling MMA8541 sensor.\r\n", CLS1_GetStdio()->stdOut);
     if (MMA1_Enable()!=ERR_OK) {
       CLS1_SendStr((uint8_t*)"ERROR: Failed enabling MMA8541!\r\n", CLS1_GetStdio()->stdErr);
     }
   }
#endif
  for(;;) {
#if 0 && PL_CONFIG_HAS_AMG8833
    /* max 10 Hz */
    if (AMG88xx_readPixels(AMGBuf, sizeof(AMGBuf)/sizeof(AMGBuf[0]))!=ERR_OK) {
      CLS1_SendStr((uint8_t*)"Failed AMG88xx_readPixels()!\r\n", CLS1_GetStdio()->stdErr);
    }
#endif
#if PL_CONFIG_HAS_SGP30
    res = SGP30_IAQmeasure(&tvoc, &co2);
    if (res!=ERR_OK) {
      CLS1_SendStr((uint8_t*)"Failed SGP30_IAQmeasure()!\r\n", CLS1_GetStdio()->stdErr);
    } else {
     // CLS1_printfIO(CLS1_GetStdio(), "TVOC %5d ppb, eCO2 %5d ppm\n", tvoc, co2);
    }
    cntr++;
    if (cntr==30) {
      cntr = 0;
      res = SGP30_GetIAQBaseline(&TVOC_base, &eCO2_base);
      if (res!=ERR_OK) {
        CLS1_SendStr((uint8_t*)"Failed SGP30_GetIAQBaseline()!\r\n", CLS1_GetStdio()->stdErr);
      } else {
        CLS1_printfIO(CLS1_GetStdio(), "*** Baseline TVOC: 0x%4x, eCO2 0x%4x\n", TVOC_base, eCO2_base);
      }
    }
#endif
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void SENSOR_Init(void) {
  if (xTaskCreate(
        SensorTask,  /* pointer to the task */
        "Sensor", /* task name for kernel awareness debugging */
        700/sizeof(StackType_t), /* task stack size */
        (void*)NULL, /* optional task startup argument */
        tskIDLE_PRIORITY+2,  /* initial priority */
        (xTaskHandle*)NULL /* optional task handle to create */
      ) != pdPASS) {
    /*lint -e527 */
    for(;;){}; /* error! probably out of memory */
    /*lint +e527 */
  }
#if PL_CONFIG_HAS_SHT71
  if (xTaskCreate(
        SHT71Task,  /* pointer to the task */
        "SHT71", /* task name for kernel awareness debugging */
        600/sizeof(StackType_t), /* task stack size */
        (void*)NULL, /* optional task startup argument */
        tskIDLE_PRIORITY+3,  /* initial priority */
        (xTaskHandle*)NULL /* optional task handle to create */
      ) != pdPASS) {
    /*lint -e527 */
    for(;;){}; /* error! probably out of memory */
    /*lint +e527 */
  }
  sensorsem = xSemaphoreCreateRecursiveMutex();
  if (sensorsem==NULL) { /* creation failed? */
    for(;;);
  }
  vQueueAddToRegistry(sensorsem, "sensorsem");
#endif
}
