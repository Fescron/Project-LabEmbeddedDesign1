/***************************************************************************//**
 * @file accel.c
 * @brief All code for the ADXL362 accelerometer.
 * @version 1.2
 * @author Brecht Van Eeckhoudt
 ******************************************************************************/


#include <stdint.h>  /* (u)intXX_t */
#include <stdbool.h> /* "bool", "true", "false" */

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

#include "../inc/accel.h"


/* Global variables */

/*
 * The “volatile” type indicates to the compiler that the data is not normal memory,
 * and could actually change at unexpected times. Hardware registers are often volatile,
 * and so are variables which get changed in interrupts.
 */

volatile int8_t XYZDATA[3] = { 0x00, 0x00, 0x00 };
uint8_t range = 0;


/**************************************************************************//**
 * @brief
 *   Initialize USART0 in SPI mode according to the settings required
 *   by the accelerometer.
 *
 * @details
 *   Enable clocks, configure pins, configure USART0 in SPI mode, route
 *   the pins, enable USART0 and set CS high.
 *****************************************************************************/
void initADXL_SPI (void)
{
	/* Enable necessary clocks */
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_USART0, true);

	/* Configure GPIO */
	GPIO_PinModeSet(ADXL_CLK_PORT, ADXL_CLK_PIN, gpioModePushPull, 0);   /* US0_CLK is push pull */
	GPIO_PinModeSet(ADXL_NCS_PORT, ADXL_NCS_PIN, gpioModePushPull, 1);   /* US0_CS is push pull */
	GPIO_PinModeSet(ADXL_MOSI_PORT, ADXL_MOSI_PIN, gpioModePushPull, 1); /* US0_TX (MOSI) is push pull */
	GPIO_PinModeSet(ADXL_MISO_PORT, ADXL_MISO_PIN, gpioModeInput, 1);    /* US0_RX (MISO) is input */

	/* Start with default config */
	USART_InitSync_TypeDef config = USART_INITSYNC_DEFAULT;

	/* Modify some settings */
	config.enable       = false;           	/* making sure to keep USART disabled until we've set everything up */
	config.refFreq      = 0;			 	/* USART/UART reference clock assumed when configuring baud rate setup. Set to 0 to use the currently configured reference clock. */
	config.baudrate     = 4000000;         	/* CLK freq is 1 MHz (1000000) */
	config.databits     = usartDatabits8;	/* master mode */
	config.master       = true;            	/* master mode */
	config.msbf         = true;            	/* send MSB first */
	config.clockMode    = usartClockMode0; 	/* clock idle low, sample on rising/first edge (Clock polarity/phase mode = CPOL/CPHA) */
	config.prsRxEnable = false;				/* If enabled: Enable USART Rx via PRS. */
	config.autoTx = false; 					/* If enabled: Enable AUTOTX mode. Transmits as long as RX is not full. Generates underflows if TX is empty. */
	config.autoCsEnable = false;            /* CS pin controlled by hardware, not firmware */

	/* Initialize USART0 with the configured parameters */
	USART_InitSync(USART0, &config);

	/* Set the pin location for USART0 */
	USART0->ROUTE = USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_LOCATION_LOC0;

	/* Enable USART0 */
	USART_Enable(USART0, usartEnable);

	/* Set CS high (active low!) */
	GPIO_PinOutSet(gpioPortE, 13);
}


/**************************************************************************//**
 * @brief
 *   Read an SPI byte from the accelerometer (8 bits) using a given address.
 *
 * @param[in] address
 *   The register address to read from.
 *
 * @return
 *   The response (one byte, uint8_t).
 *****************************************************************************/
uint8_t readADXL (uint8_t address)
{
	uint8_t response;

	/* Set CS low (active low!) */
	GPIO_PinOutClear(ADXL_NCS_PORT, ADXL_NCS_PIN);

	/* 3-byte operation according to datasheet */
	USART_SpiTransfer(USART0, 0x0B);            /* "read" instruction */
	USART_SpiTransfer(USART0, address);         /* Address */
	response = USART_SpiTransfer(USART0, 0x00); /* Read response */

	/* Set CS high */
	GPIO_PinOutSet(ADXL_NCS_PORT, ADXL_NCS_PIN);

	return (response);
}


/**************************************************************************//**
 * @brief
 *   Write an SPI byte to the accelerometer (8 bits) using a given address
 *   and specified data.
 *
 * @param[in] address
 *   The register address to write the data to (one byte, uint8_t).
 *
 * @param[in] data
 *   The data to write to the address (one byte, uint8_t).
 *****************************************************************************/
void writeADXL (uint8_t address, uint8_t data)
{
	/* Set CS low (active low!) */
	GPIO_PinOutClear(ADXL_NCS_PORT, ADXL_NCS_PIN);

	/* 3-byte operation according to datasheet */
	USART_SpiTransfer(USART0, 0x0A);    /* "write" instruction */
	USART_SpiTransfer(USART0, address); /* Address */
	USART_SpiTransfer(USART0, data);    /* Data */

	/* Set CS high */
	GPIO_PinOutSet(ADXL_NCS_PORT, ADXL_NCS_PIN);
}


/**************************************************************************//**
 * @brief
 *   Read the X-Y-Z data registers in the XYZDATA[] field using burst reads.
 *
 * @details
 *   Response data gets put in XYZDATA array (global volatile variable).
 *****************************************************************************/
void readADXL_XYZDATA (void)
{
	/* CS low (active low!) */
	GPIO_PinOutClear(ADXL_NCS_PORT, ADXL_NCS_PIN);

	/* burst read (address auto-increments) */
	USART_SpiTransfer(USART0, 0x0B);				/* "read" instruction */
	USART_SpiTransfer(USART0, ADXL_REG_XDATA); 		/* Address */
	XYZDATA[0] = USART_SpiTransfer(USART0, 0x00);	/* Read response */
	XYZDATA[1] = USART_SpiTransfer(USART0, 0x00);	/* Read response */
	XYZDATA[2] = USART_SpiTransfer(USART0, 0x00);	/* Read response */

	/* CS high */
	GPIO_PinOutSet(ADXL_NCS_PORT, ADXL_NCS_PIN);
}


/**************************************************************************//**
 * @brief
 *   Configure the measurement range and store the selected one in
 *   a global variable.
 *
 * @param[in] givenRange
 *   @li 0 - +- 2g
 *   @li 1 - +- 4g
 *   @li 2 - +- 8g
 *****************************************************************************/
void configADXL_range (uint8_t givenRange)
{
	/* Set measurement range (first two bits) */
	if (givenRange == 0) {
		writeADXL(ADXL_REG_FILTER_CTL, 0b00010011);
		range = 0;
	}
	else if (givenRange == 1) {
		writeADXL(ADXL_REG_FILTER_CTL, 0b01010011);
		range = 1;
	}
	else if (givenRange == 2) {
		writeADXL(ADXL_REG_FILTER_CTL, 0b10010011);
		range = 2;
	}
}


/**************************************************************************//**
 * @brief
 *   Configure the accelerometer to work in activity threshold mode.
 *
 * @details
 *   Route ACT to INT1 pin using INTMAP1, isolate bits and write
 *   settings to both threshold registers.
 *
 * @param[in] threshold
 *   This needs to be an unsigned 11bit number.
 *   TODO: Change to using "g" value using formula.
 *****************************************************************************/
void configADXL_activity (uint16_t threshold)
{
	/* Map INT1 pin to activity detector */
	writeADXL(ADXL_REG_INTMAP1, 0b00010000);

	/* Isolate bits using masks and shifting */
	uint8_t low  = (threshold & 0b0011111111);
	uint8_t high = (threshold & 0b1100000000) >> 8;

	/* Set threshold register values (total: 11bit unsigned) */
	writeADXL(ADXL_REG_THRESH_ACT_L, low); /* 7:0 bits used */
	writeADXL(ADXL_REG_THRESH_ACT_H, high); /* 2:0 bits used */
}


/**************************************************************************//**
 * @brief
 *   Soft reset accelerometer.
 *****************************************************************************/
void softResetADXL (void)
{
	writeADXL(ADXL_REG_SOFT_RESET, 0x52); /* 0x52 = "R" */
}


/**************************************************************************//**
 * @brief Check if the ID is correct
 *
 * @return
 *   @li true - Correct ID returned.
 *   @li false - Incorrect ID returned.
 *****************************************************************************/
bool checkID_ADXL (void)
{
	return (readADXL(ADXL_REG_DEVID_AD) == 0xAD);
}


/**************************************************************************//**
 * @brief
 *   Convert sensor value in +-g range to g value.
 *
 * @note
 *   Info found at http://ozzmaker.com/accelerometer-to-g/
 *
 * @param[in] sensorValue
 *   Value in g-range returned by sensor.
 *
 * @return
 *   The calculated g value.
 *****************************************************************************/
int32_t convertGRangeToGValue (int8_t sensorValue)
{
	if (range == 0)
	{
		return ((2*2*1000 / 255) * sensorValue);
	}
	else if (range == 1)
	{
		return ((2*4*1000 / 255) * sensorValue);
	}
	else if (range == 2)
	{
		return ((2*8*1000 / 255) * sensorValue);
	}
	return (0);
}
