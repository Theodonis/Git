/*
 * AirSensor.c
 *
 *  Created on: 21.11.2018
 *      Author: Theo
 */

#include "Platform.h"
#if PL_CONFIG_HAS_SHT71
#include "SHT71.h"
#include "I2C1.h"
#include "I2C1config.h"
#include "UTIL1.h"
#include "SDA1.h"
#include "SCL1.h"
#include "Wait1.h"
#if I2C1_HAS_RTOS
  /* include RTOS header files */
  #include "FreeRTOS.h" /* for yielding */
  #include "task.h"
#endif


#define SHT71_DEFAULT_ADDR				(0x0)
#define SHT71_COMMAND_MESURE_TEMP		(0b00000011)
#define SHT71_COMMAND_MESURE_HUMIDITY	(0b00000101)
#define SHT71_COMMAND_READ_ST_REGISTER	(0x7)
#define SHT71_COMMAND_WRITE_ST_REGISTER	(0x6)
#define SHT71_COMMAND_SOFT_RESET		(0x1E)

#define SHT71_CRC8_INIT 				(0xFF)
#define SHT71_CRC8_POLYNOMIAL			(0x31)

#define OUTPUT      1U
#define INPUT       0U
#define WRITE       0U
#define READ        1U
#define TRIALS      25

uint8_t SHT71_GenerateCRC(uint8_t *data, uint8_t datalen) {
  /* calculates 8-Bit checksum with given polynomial */
  uint8_t crc = SHT71_CRC8_INIT;
  uint8_t b;
  uint8_t i;

  for (i=0; i<datalen; i++) {
    crc ^= data[i];
    for (b=0; b<8; b++) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ SHT71_CRC8_POLYNOMIAL;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}


void SHT71_StartTrans(void) {

	uint16_t timeout;


	SDA_SetDir((bool)INPUT);     /* SDA HIGH - START SETUP*/
	SCL_SetDir((bool)INPUT);     /* CLOCK HIGH PULSE */
	Delay();                       /* CLOCK HIGH PULSE & BUS FREE TIME */
	/* check that we have a valid start condition: SDA needs to be high */
	timeout = I2C1_CONFIG_TIMEOUT_COUNTER_VALUE;
	while((SDA_GetVal()==0U)&&(timeout!=0U)) { /* WAIT FOR CLOCK HIGH PULSE */
		timeout--;
		I2C1_OSYIELD();
	}
	Delay();
	if (timeout==0) {
		InternalStop();
	 //   return ERR_BUSY;
	}

	SDA_SetDir((bool)OUTPUT);
	SDA_ClrVal();                	/* START CONDITION */
	Delay();                       /* START HOLD TIME */
	SCL_SetDir((bool)OUTPUT);
	SCL_ClrVal();                	/* CLOCK LOW PULSE */
	Delay();

	SCL_SetDir((bool)INPUT);        /* CLOCK HIH PULSE */
	Delay();
	SDA_SetDir((bool)INPUT);		/* second START CONDITION  SDA High*/
	Delay();
	SCL_SetDir((bool)OUTPUT);
	SCL_ClrVal();                	/* CLOCK LOW PULSE */
}

void SHT71_Reset(void){
	SHT71_StartTrans();
	Write(SHT71_COMMAND_SOFT_RESET);
}


uint8_t SHT71_ReadTempHum(float *temperature,float *humidity) {
  double stemp, shum, shum_squer;
  uint16_t ST, SRH;
  uint16_t timeout;
  uint8_t readbuffer[2];
  uint8_t Address;
  uint16_t Trial;
  bool Acknowledge;

  Trial = TRIALS;
  timeout = I2C1_CONFIG_TIMEOUT_COUNTER_VALUE;

  /* measure_air_humidity */
  Address = SHT71_COMMAND_MESURE_HUMIDITY;

  do {
      SHT71_StartTrans();
      Write(Address);
	  Acknowledge = GetAck();
	  --Trial;
  } while ((Trial != 0U) && Acknowledge);
  Delay();
  SCL_SetDir((bool)OUTPUT);
  SCL_ClrVal();                	/* CLOCK LOW PULSE */

  WAIT1_WaitOSms(75);

  while((!(SDA_GetVal())==0U)&&(timeout!=0U)) { /* WAIT FOR  LOW DATA */
	  timeout--;
	  I2C1_OSYIELD();
  }
  readbuffer[1]=Read();
  I2C1_SendAck(0);
  readbuffer[0]=Read();
  SRH = ((readbuffer[1])<<8)|readbuffer[0];


  /* measure_air_temperature */
  Address =SHT71_COMMAND_MESURE_TEMP;
  do {
	 SHT71_StartTrans();
	 Write(Address);
	 Acknowledge = GetAck();
	 --Trial;
  } while ((Trial != 0U) && Acknowledge);
  Delay();
  SCL_SetDir((bool)OUTPUT);
  SCL_ClrVal();                	/* CLOCK LOW PULSE */
  WAIT1_WaitOSms(320);
  while((!(SDA_GetVal())==0U)&&(timeout!=0U)) { /* WAIT FOR Low Data */
	  timeout--;
	  I2C1_OSYIELD();
  }
  readbuffer[1]=Read();
  I2C1_SendAck(0);
  readbuffer[0]=Read();
  ST = ((readbuffer[1])<<8)|readbuffer[0];




  /* calculate_air_temperature */
  stemp 	= ST;
  stemp 	*= 0.01;
  stemp 	= -39.6 	+ stemp;

  *temperature = stemp;

  /* calculate_air_humidity */
  shum 			= SRH;
  shum_squer 	= shum*shum;
  shum 			*= 0.0367;
  shum_squer 	*= 0.0000015955;

  shum = -2.0468 + shum - shum_squer;

  *humidity = shum;

  return ERR_OK;
}



#if SHT71_CONFIG_PARSE_COMMAND_ENABLED
static uint8_t PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[32];
  float temperature, humidity;

  CLS1_SendStatusStr((unsigned char*)"SHT71", (unsigned char*)"\r\n", io->stdOut);

  if(SHT71_ReadTempHum(&temperature, &humidity)==ERR_OK) {
    UTIL1_NumFloatToStr(buf, sizeof(buf), temperature, 2);
    UTIL1_strcat(buf, sizeof(buf), "°C\r\n");
    CLS1_SendStatusStr((unsigned char*)"  temp", buf, io->stdOut);

    UTIL1_NumFloatToStr(buf, sizeof(buf), humidity, 2);
    UTIL1_strcat(buf, sizeof(buf), " RH\r\n");
    CLS1_SendStatusStr((unsigned char*)"  humidity", buf, io->stdOut);
  } else {
    CLS1_SendStatusStr("  ERROR:", (unsigned char*)"  *** ERROR reading sensor values\r\n", io->stdOut);
  }
  return ERR_OK;
}

static uint8_t PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"SHT71", (unsigned char*)"Group of SHT71 commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Print help or status information\r\n", io->stdOut);
  return ERR_OK;
}
#endif /* SHT31_CONFIG_PARSE_COMMAND_ENABLED */

#if SHT71_CONFIG_PARSE_COMMAND_ENABLED
uint8_t SHT71_ParseCommand(const unsigned char* cmd, bool *handled, const CLS1_StdIOType *io) {
  uint8_t res = ERR_OK;

  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP) == 0
    || UTIL1_strcmp((char*)cmd, "SHT71 help") == 0)
  {
    *handled = TRUE;
    return PrintHelp(io);
  } else if (   (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0)
             || (UTIL1_strcmp((char*)cmd, "SHT71 status")==0)
            )
  {
    *handled = TRUE;
    res = PrintStatus(io);
  }
  return res;
}
#endif /* SHT71_CONFIG_PARSE_COMMAND_ENABLED */

void SHT71_Init(void){
	SHT71_Reset();
}
#endif
