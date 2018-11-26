/***************************************************************************//**
 * @file accel.c
 * @brief All code for the ADXL362 accelerometer.
 * @version 1.0
 * @author Brecht Van Eeckhoudt
 ******************************************************************************/

#include <stdint.h>  	/* (u)intXX_t */
#include <stdbool.h> 	/* "bool", "true", "false" */

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

/**************************************************************************//**
 * @brief Initialize USART0
 *****************************************************************************/
void initADXL_SPI (void)
{
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_USART0, true);

	/* Configure GPIO mode and so are variables which get changed in interrupts. */
	GPIO_PinModeSet(ADXL_CLK_PORT, ADXL_CLK_PIN, gpioModePushPull, 0);   /* US0_CLK is push pull */
	GPIO_PinModeSet(ADXL_NCS_PORT, ADXL_NCS_PIN, gpioModePushPull, 1); 	 /* US0_CS is push pull */
	GPIO_PinModeSet(ADXL_MOSI_PORT, ADXL_MOSI_PIN, gpioModePushPull, 1); /* US0_TX (MOSI) is push pull */
	GPIO_PinModeSet(ADXL_MISO_PORT, ADXL_MISO_PIN, gpioModeInput, 1);    /* US0_RX (MISO) is input */

	/* Start with default config, then modify as necessary */
	USART_InitSync_TypeDef config = USART_INITSYNC_DEFAULT;

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

	USART_InitSync(USART0, &config);

	/* Set and enable USART pin locations */
	USART0->ROUTE = USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_LOCATION_LOC0;

	/* Enable USART0 */
	USART_Enable(USART0, usartEnable);

	/* CS high (active low!) */
	GPIO_PinOutSet(gpioPortE, 13);
}

/**************************************************************************//**
 * @brief Read an SPI byte from the accelerometer (8 bits)
 *
 * @param address Register address
 * @return Response
 *****************************************************************************/
uint8_t readADXL (uint8_t address)
{
	uint8_t response;

	/* CS low (active low!) */
	GPIO_PinOutClear(ADXL_NCS_PORT, ADXL_NCS_PIN);

	/* 3-byte operation */
	USART_SpiTransfer(USART0, 0x0B);			/* "read" instruction */
	USART_SpiTransfer(USART0, address);			/* Address */
	response = USART_SpiTransfer(USART0, 0x00);	/* Read response */

	/* CS high */
	GPIO_PinOutSet(ADXL_NCS_PORT, ADXL_NCS_PIN);

	return (response);
}

/**************************************************************************//**
 * @brief Write an SPI byte to the accelerometer (8 bits)
 *
 * @param address Register address
 * @param data Data
 *****************************************************************************/
void writeADXL (uint8_t address, uint8_t data)
{
	/* CS low (active low!) */
	GPIO_PinOutClear(ADXL_NCS_PORT, ADXL_NCS_PIN);

	/* 3-byte operation */
	USART_SpiTransfer(USART0, 0x0A);	/* "write" instruction */
	USART_SpiTransfer(USART0, address);	/* Address */
	USART_SpiTransfer(USART0, data);	/* Data */

	/* CS high */
	GPIO_PinOutSet(ADXL_NCS_PORT, ADXL_NCS_PIN);
}

/**************************************************************************//**
 * @brief Read the X-Y-Z data registers in the XYZDATA[] field using burst reads
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
 * @brief Set the measurement range (0 = +- 2g, 1 = +- 4g, 2 = +- 8g)
 *****************************************************************************/
void setRangeADXL (uint8_t range)
{
	/* Set measurement range (first two bits) */
	if (range == 0) {
		writeADXL(ADXL_REG_FILTER_CTL, 0b00010011);
	}
	else if (range == 1) {
		writeADXL(ADXL_REG_FILTER_CTL, 0b01010011);
	}
	else if (range == 2) {
		writeADXL(ADXL_REG_FILTER_CTL, 0b10010011);
	}
}

/**************************************************************************//**
 * @brief Soft reset
 *****************************************************************************/
void softResetADXL (void)
{
	writeADXL(ADXL_REG_SOFT_RESET, 0x52); /* 0x52 = "R" */
}

/**************************************************************************//**
 * @brief Check if the ID is correct
 *
 * @return True if the ID is correct
 *****************************************************************************/
bool checkID_ADXL (void)
{
	return (readADXL(ADXL_REG_DEVID_AD) == 0xAD);
}

/**************************************************************************//**
 * @brief Converts sensor value in +-g range to g value (http://ozzmaker.com/accelerometer-to-g/)
 *
 * @param sensorValue value in g-range returned by sensor
 * @param sensitivity maximum value the sensor data returns
 * @return g value
 *****************************************************************************/
int32_t convertGRangeToGValue (int8_t sensorValue, int8_t sensitivity)
{
	if (sensitivity == 0)
	{
		return ((2*2*1000 / 255) * sensorValue);
	}
	else if (sensitivity == 1)
	{
		return ((2*4*1000 / 255) * sensorValue);
	}
	else if (sensitivity == 2)
	{
		return ((2*8*1000 / 255) * sensorValue);
	}
	return (0);
}
