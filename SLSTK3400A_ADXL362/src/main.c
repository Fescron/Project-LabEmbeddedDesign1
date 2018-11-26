/**************************************************************************//**
 * @main_series0_HG.c
 * @brief Demonstrates USART1 as SPI master.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdint.h>  /* (u)intXX_t */
#include <stdbool.h> /* "bool", "true", "false" */
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

#include "../inc/accel.h"

/* Comment the two lines below to remove all USART1 debugging stuff (TX = PC0 @ 115200) */
#include "dbprint.h"
#define DEBUGGING /* Comment to remove all USART1 stuff */

/*  PE10: MOSI
 *  PE11: MISO
 *  PE12: CLK
 *  PD4: NCS
 */



/* Global variables */
static volatile uint32_t msTicks; /* Static volatile because it's a global variable that's modified by an interrupt service routine (https://barrgroup.com/Embedded-Systems/How-To/C-Volatile-Keyword) */

/* Prototypes */
static void Delay(uint32_t dlyTicks);


/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
	msTicks++;       /* increment counter necessary in Delay()*/
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
	uint32_t curTicks;

	curTicks = msTicks;
	while ((msTicks - curTicks) < dlyTicks) ;
}

/**************************************************************************//**
 * @brief Initialize the LED's
 *****************************************************************************/
void initLEDS (void)
{
	GPIO_PinModeSet(gpioPortF, 4, gpioModePushPull, 1); /* LED0 */
	GPIO_PinModeSet(gpioPortF, 5, gpioModePushPull, 1); /* LED1 */
	GPIO_PinOutClear(gpioPortF, 4); /* Disable LED0 */
	GPIO_PinOutClear(gpioPortF, 5); /* Disable LED1 */
}

/**************************************************************************//**
 * @brief Error method
 * Flashes the LEDS, displays a UART message & holds the MCU forever in a loop until a reset.
 *****************************************************************************/
void Error (void)
{
#ifdef DEBUGGING /* DEBUGGING */
	dbprintln(">>>>> Error! Please reset MCU <<<<<");
#endif /* DEBUGGING */

	GPIO_PinOutClear(gpioPortF, 4); /* Disable LED0 */
	GPIO_PinOutSet(gpioPortF, 5);   /* Enable LED1 */

	while(1)
	{
		Delay(100);
		GPIO_PinOutToggle(gpioPortF, 4); /* Toggle LED0 */
		GPIO_PinOutToggle(gpioPortF, 5); /* Toggle LED1 */
	}
}

/**************************************************************************//**
 * @brief Go through all of the register settings to see the influence it has on power usage
 *****************************************************************************/
void testADXL (void)
{
#ifdef DEBUGGING /* DEBUGGING */
	dbprintln("INFO: Testing the ADXL (all in +-2g default measurement range, 7 seconds):");
	dbprintln("   standby - 1sec - ODR 12,5Hz + enable measurements - 1sec - ODR 25 Hz");
	dbprintln("   - 1sec - ODR 50 Hz - 1sec - ODR 100 Hz (default on reset) - 1sec -");
	dbprintln("   200Hz - 1sec - ODR 400 Hz - 1sec - soft Reset");
#endif /* DEBUGGING */

	/* Soft reset ADXL */
	softResetADXL();

	/* Standby */
	Delay(1000);

	/* ODR 12,5Hz */
	writeADXL(ADXL_REG_FILTER_CTL, 0b00010000); /* last 3 bits are ODR */

	/* Enable measurements */
	writeADXL(ADXL_REG_POWER_CTL, 0b00000010); /* last 2 bits are measurement mode */

	Delay(1000);

	/* ODR 25Hz */
	writeADXL(ADXL_REG_FILTER_CTL, 0b00010001); /* last 3 bits are ODR */
	Delay(1000);

	/* ODR 50Hz */
	writeADXL(ADXL_REG_FILTER_CTL, 0b00010010); /* last 3 bits are ODR */
	Delay(1000);

	/* ODR 100Hz (default) */
	writeADXL(ADXL_REG_FILTER_CTL, 0b00010011); /* last 3 bits are ODR */
	Delay(1000);

	/* ODR 200Hz */
	writeADXL(ADXL_REG_FILTER_CTL, 0b00010100); /* last 3 bits are ODR */
	Delay(1000);

	/* ODR 400Hz */
	writeADXL(ADXL_REG_FILTER_CTL, 0b00010101); /* last 3 bits are ODR */
	Delay(1000);

	/* Soft reset ADXL */
	softResetADXL();

#ifdef DEBUGGING /* DEBUGGING */
	dbprintln("INFO: Testing done");
#endif /* DEBUGGING */

}

/**************************************************************************//**
 * @brief Go through all of the register settings to see the influence it has on power usage
 *****************************************************************************/
void resetHandlerADXL (void)
{
	/* Soft reset ADXL */
	softResetADXL();

	/* Read DEVID_AD */
	if (checkID_ADXL())
	{
#ifdef DEBUGGING /* DEBUGGING */
		dbprintln("INFO: Soft reset ADXL -- correct ID!");
#endif /* DEBUGGING */
	}
	else
	{
#ifdef DEBUGGING /* DEBUGGING */
		dbprintln("WARN: Soft reset ADXL -- Incorrect ID!");
#endif /* DEBUGGING */

		Delay(1000);

		/* Soft reset */
		softResetADXL();

		if (checkID_ADXL())
		{
#ifdef DEBUGGING /* DEBUGGING */
			dbprintln("INFO: Retry soft reset -- correct ID!");
#endif /* DEBUGGING */
		}
		else
		{
#ifdef DEBUGGING /* DEBUGGING */
			dbprintln("CRIT: Retry soft reset -- Incorrect ID!");
#endif /* DEBUGGING */
			Error();
		}
	}
}


/**************************************************************************//**
 * @brief Main function
 *****************************************************************************/
int main(void)
{
	uint32_t counter = 0;

	/* Initialize chip */
	CHIP_Init();

	/* Initalize systick */
	if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

	/* Initialize USART0 as SPI slave (also initialize CS pin) */
	initADXL_SPI();

	/* Initialize LED's */
	initLEDS();

#ifdef DEBUGGING /* DEBUGGING */
	dbprint_INIT(USART1, 4, true, false);
	dbprintln("### Initialisations complete ###");
#endif /* DEBUGGING */


	/* Soft reset ADXL handler */
	resetHandlerADXL();

	/*
	 * Profile the ADXL
	 *
	 * Energy profiler werkt ook met VCOM?
	 * 		Als VCOM open staat werkt het even, de energy profiler neemt veel tijd om op te starten,
	 * 		erna nooit meer debug messages tot USB wordt uitgetrokken & opnieuw ingestoken...
	 */

	/*
	dbPrintln("Waiting 5 seconds...");
	Delay(5000);
	dbPrintln("Starting...");
	testADXL();
	*/

	/* Set the measurement range (0 = +- 2g, 1 = +- 4g, 2 = +- 8g) */
	uint8_t range = 1;
	setRangeADXL(range);

#ifdef DEBUGGING /* DEBUGGING */
	if (range == 0)
	{
		dbprintln("INFO: Measurement mode +- 2g selected");
	}
	else if (range == 1)
	{
		dbprintln("INFO: Measurement mode +- 4g selected");
	}
	else if (range == 2)
	{
		dbprintln("INFO: Measurement mode +- 8g selected");
	}
#endif /* DEBUGGING */


	/* Enable measurements */
	writeADXL(ADXL_REG_POWER_CTL, 0b00000010); /* last 2 bits are measurement mode */
	dbprintln("INFO: Measurement enabled");

	while(1)
	{
		GPIO_PinOutSet(gpioPortF, 4); /* Enable LED0 */


		/* Read XYZ sensor data */
		readADXL_XYZDATA();


		/* Print XYZ sensor data */
		dbprint("\r[");
		dbprintUint(counter);
		dbprint("] X: ");
		dbprintInt(10, convertGRangeToGValue(XYZDATA[0], range));
		dbprint(" mg | Y: ");
		dbprintInt(10, convertGRangeToGValue(XYZDATA[1], range));
		dbprint(" mg | Z: ");
		dbprintInt(10, convertGRangeToGValue(XYZDATA[2], range));
		dbprint(" mg   "); /* Overwrite other data if it's remaining (see \r) */
		/* dbprintln(""); */

		GPIO_PinOutClear(gpioPortF, 4); /* Disable LED0 */

		counter++;

		Delay(1000);
	}
}
