/***************************************************************************//**
 * @file main.c
 * @brief The main file for the program to interface to the accelerometer.
 * @details Started with code from the UART example (main_series0_HG.c) from SiLabs Github.
 * @version 1.2
 * @author Brecht Van Eeckhoudt
 *
 * ******************************************************************************
 *
 * @section Pinout ADXL362
 *
 *   PE10: MOSI
 *   PE11: MISO
 *   PE12: CLK
 *   PD4: NCS
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


/* Comment the two lines below to remove all USART1 debugging stuff (TX = PC0 ~ VCOM @ 115200) */
#include "dbprint.h"
#define DEBUGGING /* Comment to remove all USART1 stuff */


/* Global variables */
static volatile uint32_t msTicks; /* Volatile because it's a global variable that's modified by an interrupt service routine */


/* Prototypes */
static void Delay (uint32_t dlyTicks); /* Static so the function is only "seen" in the file it's declared in. */


/**************************************************************************//**
 * @brief
 *   Interrupt Service Routine for system tick counter.
 *****************************************************************************/
void SysTick_Handler (void)
{
	msTicks++; /* Increment counter necessary in Delay() */
}


/**************************************************************************//**
 * @brief
 *   Waits a certain amount of milliseconds using the systicks.
 *
 * @param[in] dlyTicks
 *   Number of milliseconds (ticks) to wait.
 *****************************************************************************/
void Delay (uint32_t dlyTicks)
{
	uint32_t curTicks = msTicks;

	while ((msTicks - curTicks) < dlyTicks);
}


/**************************************************************************//**
 * @brief
 *   Initialize the onboard LED's.
 *****************************************************************************/
void initLEDS (void)
{
	GPIO_PinModeSet(gpioPortF, 4, gpioModePushPull, 1); /* LED0 */
	GPIO_PinModeSet(gpioPortF, 5, gpioModePushPull, 1); /* LED1 */
	GPIO_PinOutClear(gpioPortF, 4); /* Disable LED0 */
	GPIO_PinOutClear(gpioPortF, 5); /* Disable LED1 */
}


/**************************************************************************//**
 * @brief
 *   Error method.
 *
 * @details
 *   Flashes the LED's, displays a UART message and holds
 *   the microcontroller forever in a loop until it gets reset.
 *
 * @param[in] number
 *   The number to indicate where in the code the error was thrown.
 *****************************************************************************/
void Error (uint8_t number)
{

#ifdef DEBUGGING /* DEBUGGING */
	dbprint(">>> Error (");
	dbprintInt(number);
	dbprintln(")! Please reset MCU. <<<");
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
 * @brief
 *   Go through all of the register settings to see the influence
 *   they have on power usage.
 *****************************************************************************/
void testADXL (void)
{

#ifdef DEBUGGING /* DEBUGGING */
		dbprintln("Waiting 5 seconds...");
#endif /* DEBUGGING */

		Delay(5000);

#ifdef DEBUGGING /* DEBUGGING */
		dbprintln("Starting...");
#endif /* DEBUGGING */

#ifdef DEBUGGING /* DEBUGGING */
	dbinfo("Testing the ADXL (all in +-2g default measurement range, 7 seconds):");
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
	dbinfo("Testing done");
#endif /* DEBUGGING */

}


/**************************************************************************//**
 * @brief
 *   Soft reset accelerometer handler.
 *
 * @details
 *   If the first ID check fails, the MCU is put on hold for one second
 *   and the ID gets checked again.
 *****************************************************************************/
void resetHandlerADXL (void)
{
	/* Soft reset ADXL */
	softResetADXL();

	/* Read DEVID_AD */
	if (checkID_ADXL())
	{

#ifdef DEBUGGING /* DEBUGGING */
		dbinfo("Soft reset ADXL -- correct ID!");
#endif /* DEBUGGING */

	}
	else
	{

#ifdef DEBUGGING /* DEBUGGING */
		dbwarn("Soft reset ADXL -- Incorrect ID!");
#endif /* DEBUGGING */

		Delay(1000);

		/* Soft reset */
		softResetADXL();

		if (checkID_ADXL())
		{

#ifdef DEBUGGING /* DEBUGGING */
			dbinfo("Retry soft reset -- correct ID!");
#endif /* DEBUGGING */

		}
		else
		{

#ifdef DEBUGGING /* DEBUGGING */
			dbcrit("Retry soft reset -- Incorrect ID!");
#endif /* DEBUGGING */

			Error(0);
		}
	}
}


/**************************************************************************//**
 * @brief
 *   Main function.
 *****************************************************************************/
int main(void)
{
	uint32_t counter = 0;

	/* Initialize chip */
	CHIP_Init();

	/* Initialize systick */
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

	/* Profile the ADXL (make sure to not use VCOM here!) */
	//testADXL();

	/* Set the measurement range (0 = +- 2g, 1 = +- 4g, 2 = +- 8g) */
	uint8_t setRange = 1;

	configADXL_range(setRange);

#ifdef DEBUGGING /* DEBUGGING */
	if (setRange == 0)
	{
		dbinfo("Measurement mode +- 2g selected");
	}
	else if (setRange == 1)
	{
		dbinfo("Measurement mode +- 4g selected");
	}
	else if (setRange == 2)
	{
		dbinfo("Measurement mode +- 8g selected");
	}
#endif /* DEBUGGING */


	/* Enable measurements */
	writeADXL(ADXL_REG_POWER_CTL, 0b00000010); /* Last 2 bits are measurement mode */
	dbinfo("Measurement enabled");

	while(1)
	{
		GPIO_PinOutSet(gpioPortF, 4); /* Enable LED0 */


		/* Read XYZ sensor data */
		readADXL_XYZDATA();


		/* Print XYZ sensor data */
		dbprint("\r[");
		dbprintInt(counter);
		dbprint("] X: ");
		dbprintInt(convertGRangeToGValue(XYZDATA[0]));
		dbprint(" mg | Y: ");
		dbprintInt(convertGRangeToGValue(XYZDATA[1]));
		dbprint(" mg | Z: ");
		dbprintInt(convertGRangeToGValue(XYZDATA[2]));
		dbprint(" mg   "); /* Overwrite other data if it's remaining (see \r) */
		/* dbprintln(""); */

		GPIO_PinOutClear(gpioPortF, 4); /* Disable LED0 */

		counter++;

		Delay(1000);
	}
}
