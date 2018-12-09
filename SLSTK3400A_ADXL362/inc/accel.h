/***************************************************************************//**
 * @file accel.h
 * @brief All code for the ADXL362 accelerometer
 * @version 1.2
 * @author Brecht Van Eeckhoudt
 ******************************************************************************/


/* Include guards prevent multiple inclusions of the same header */
#ifndef _ACCEL_H_
#define _ACCEL_H_


#include <stdint.h>  	/* (u)intXX_t */
#include <stdbool.h> 	/* "bool", "true", "false" */


/* ADXL GPOI */
#define ADXL_CLK_PORT gpioPortE
#define ADXL_CLK_PIN 12
#define ADXL_MOSI_PORT gpioPortE
#define ADXL_MOSI_PIN 10
#define ADXL_MISO_PORT gpioPortE
#define ADXL_MISO_PIN 11
#define ADXL_NCS_PORT gpioPortD 	/* Can't use the US0_CS port (PE13) to manually set/clear CS line */
#define ADXL_NCS_PIN 4


/* ADXL REGISTERS */
#define ADXL_REG_DEVID_AD 		0x00 /* Reset: 0xAD */
#define ADXL_REG_DEVID_MST 		0x01 /* Reset: 0x1D */
#define ADXL_REG_PARTID 		0x02 /* Reset: 0xF2 */
#define ADXL_REG_REVID 			0x03 /* Reset: 0x01 (can be incremented) */
#define ADXL_REG_XDATA 			0x08
#define ADXL_REG_YDATA 			0x09
#define ADXL_REG_ZDATA 			0x0A
#define ADXL_REG_STATUS 		0x0B
#define ADXL_REG_TEMP_L 		0x14
#define ADXL_REG_TEMP_H 		0x15
#define ADXL_REG_SOFT_RESET 	0x1F /* Needs to be 0x52 ("R") written to for a soft reset */
#define ADXL_REG_THRESH_ACT_L	0x20 /* 7:0 bits used */
#define ADXL_REG_THRESH_ACT_H	0x21 /* 2:0 bits used */
#define ADXL_REG_INTMAP1 		0x2A /* INT_LOW -- AWAKE -- INACT -- ACT -- FIFO_OVERRUN -- FIFO_WATERMARK -- FIFO_READY -- DATA_READY */
#define ADXL_REG_INTMAP2 		0x2B /* INT_LOW -- AWAKE -- INACT -- ACT -- FIFO_OVERRUN -- FIFO_WATERMARK -- FIFO_READY -- DATA_READY */
#define ADXL_REG_FILTER_CTL 	0x2C /* Write FFxx xxxx (FF = 00 for +-2g, 01 for =-4g, 1x for +- 8g) for measurement range selection */
#define ADXL_REG_POWER_CTL 		0x2D /* Write xxxx xxMM (MM = 10) to: measurement mode */


/* Global variables (project-wide accessible) */
volatile int8_t XYZDATA[3];
uint8_t range;


/* Prototypes */
void initADXL_SPI (void);

uint8_t readADXL (uint8_t address);
void writeADXL (uint8_t address, uint8_t data);

void readADXL_XYZDATA (void);

void configADXL_range (uint8_t givenRange);
void configADXL_activity (uint16_t threshold);

void softResetADXL (void);
bool checkID_ADXL (void);

int32_t convertGRangeToGValue (int8_t sensorValue);


#endif /* _ACCEL_H_ */

