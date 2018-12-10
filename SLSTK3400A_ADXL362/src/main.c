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
 *   PD7: INT1
 *
 ******************************************************************************/


/* Comment the line below to remove all USART1 debugging stuff (TX = PC0 ~ VCOM @ 115200) */
#define DEBUGGING /* Comment to remove all USART1 stuff */


#include <stdint.h>    /* (u)intXX_t */
#include <stdbool.h>   /* "bool", "true", "false" */
#include "em_device.h" /* Include necessary MCU-specific header file */
#include "em_chip.h"   /* Chip Initialization */
#include "em_cmu.h"    /* Clock management unit */
#include "em_gpio.h"   /* General Purpose IO */
#include "em_usart.h"  /* Universal synchronous/asynchronous receiver/transmitter */
#include "em_emu.h"    /* Energy Management Unit */
#include "em_rtc.h"    /* Real Time Counter (RTC) */

#include "../inc/accel.h"

#ifdef DEBUGGING /* DEBUGGING */
#include "dbprint.h"
#endif /* DEBUGGING */


/* Definitions for RTC compare interrupts */
#define DELAY_RTC 60.0 /* seconds */
#define LFXOFREQ 32768
#define COMPARE_RTC (DELAY_RTC * LFXOFREQ)


/* Definitions for buttons */
#define PB0_PORT gpioPortC
#define PB0_PIN  9
#define PB1_PORT gpioPortC
#define PB1_PIN  10


/* Global variables */
static volatile uint32_t msTicks;       /* Volatile because it's a global variable that's modified by an interrupt service routine */
static volatile bool triggered = false; /* Accelerometer triggered interrupt */


/* Prototypes */
static void Delay (uint32_t dlyTicks);  /* Static so the function is only "seen" in the file it's declared in. */


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
 *   Disable SysTick interrupt and counter by clearing their bits.
 *
 * @note
 *   SysTick interrupt and counter (used by Delay) need to
 *   be disabled before going to EM2.
 *****************************************************************************/
void disableSystick (void)
{
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk & ~SysTick_CTRL_ENABLE_Msk;
}


/**************************************************************************//**
 * @brief
 *   Enable SysTick interrupt and counter by setting their bits.
 *
 * @note
 *   SysTick interrupt and counter (used by Delay) need to
 *   be enabled after waking up from EM2.
 *****************************************************************************/
void enableSystick (void)
{
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
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
 *   Initialize GPIO wakeup functionality.
 *
 * @details
 *   Initialize buttons PB0 and PB1 on falling-edge interrupts and
 *   ADXL_INT1 on rising-edge interrupts.
 *****************************************************************************/
void initWakeup (void)
{
	/* Enable necessary clock */
	CMU_ClockEnable(cmuClock_GPIO, true);

	/* Configure PB0 and PB1 as input with glitch filter enabled */
	GPIO_PinModeSet(PB0_PORT, PB0_PIN, gpioModeInputPullFilter, 1);
	GPIO_PinModeSet(PB1_PORT, PB1_PIN, gpioModeInputPullFilter, 1);

	/* Configure ADXL_INT1 as input */
	GPIO_PinModeSet(ADXL_INT1_PORT, ADXL_INT1_PIN, gpioModeInput, 1);

	/* Enable IRQ for even numbered GPIO pins */
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	/* Enable IRQ for odd numbered GPIO pins */
	NVIC_EnableIRQ(GPIO_ODD_IRQn);

	/* Enable falling-edge interrupts for PB pins */
	GPIO_IntConfig(PB0_PORT, PB0_PIN, 0, 1, true);
	GPIO_IntConfig(PB1_PORT, PB1_PIN, 0, 1, true);

	/* Enable rising-edge interrupts for ADXL_INT1 */
	GPIO_IntConfig(ADXL_INT1_PORT, ADXL_INT1_PIN, 1, 0, true);
}


/**************************************************************************//**
 * @brief
 *   GPIO Even IRQ for pushbuttons on even-numbered pins.
 *
 * @note
 *   The "weak" definition for this method is located in "system_efm32hg.h".
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
	/* Read interrupt flags */
	uint32_t flags = GPIO_IntGet();

#ifdef DEBUGGING /* DEBUGGING */
	dbinfo("Even numbered GPIO interrupt triggered.");
	if (flags == 0x400) dbprint_color("PB1\n\r", 4);
#endif /* DEBUGGING */

	/* Clear all even pin interrupt flags */
	GPIO_IntClear(0x5555);
}


/**************************************************************************//**
 * @brief
 *   GPIO Odd IRQ for pushbuttons on odd-numbered pins.
 *
 * @note
 *   The "weak" definition for this method is located in "system_efm32hg.h".
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
	/* Read interrupt flags */
	uint32_t flags = GPIO_IntGet();

#ifdef DEBUGGING /* DEBUGGING */
	dbinfo("Odd numbered GPIO interrupt triggered.");
	if (flags == 0x200) dbprint_color("PB0\n\r", 4);
	else if (flags == 0x80) dbprint_color("INT1-PD7\n\r", 4);
#endif /* DEBUGGING */

	/* Indicate that the accelerometer has given an interrupt */
	if (flags == 0x80) triggered = true;

	/* Clear all odd pin interrupt flags */
	GPIO_IntClear(0xAAAA);
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
 *   Read and display g values forever with a one second interval.
 *****************************************************************************/
void readValuesADXL (void)
{
	uint32_t counter = 0;

	/* Enable measurements */
	writeADXL(ADXL_REG_POWER_CTL, 0b00000010); /* Last 2 bits are measurement mode */

#ifdef DEBUGGING /* DEBUGGING */
	dbinfo("Measurement enabled");
#endif /* DEBUGGING */

	/* Infinite loop */
	while (1)
	{
		GPIO_PinOutSet(gpioPortF, 4); /* Enable LED0 */

		/* Read XYZ sensor data */
		readADXL_XYZDATA();

#ifdef DEBUGGING /* DEBUGGING */
		/* Print XYZ sensor data */
		dbprint("\r[");
		dbprintInt(counter);
		dbprint("] X: ");
		dbprintInt(convertGRangeToGValue(XYZDATA[0]));
		dbprint(" mg | Y: ");
		dbprintInt(convertGRangeToGValue(XYZDATA[1]));
		dbprint(" mg | Z: ");
		dbprintInt(convertGRangeToGValue(XYZDATA[2]));
		dbprint(" mg   "); /* Extra spacing is to overwrite other data if it's remaining (see \r) */
		/* dbprintln(""); */
#endif /* DEBUGGING */

		GPIO_PinOutClear(gpioPortF, 4); /* Disable LED0 */

		counter++;

		Delay(1000);
	}
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
		dbinfo("Soft reset ADXL - correct ID!");
#endif /* DEBUGGING */

	}
	else
	{

#ifdef DEBUGGING /* DEBUGGING */
		dbwarn("Soft reset ADXL - Incorrect ID!");
#endif /* DEBUGGING */

		Delay(1000);

		/* Soft reset */
		softResetADXL();

		if (checkID_ADXL())
		{

#ifdef DEBUGGING /* DEBUGGING */
			dbinfo("Retry soft reset - correct ID!");
#endif /* DEBUGGING */

		}
		else
		{

#ifdef DEBUGGING /* DEBUGGING */
			dbcrit("Retry soft reset - Incorrect ID!");
#endif /* DEBUGGING */

			Error(0);
		}
	}
}


/**************************************************************************//**
 * @brief
 *   RTCC interrupt service routine.
 *
 * @note
 *   The "weak" definition for this method is located in "system_efm32hg.h".
 *****************************************************************************/
void RTC_IRQHandler (void)
{
	/* Reset counter */
	RTC_CounterReset();

	/* Clear the interrupt source */
	RTC_IntClear(RTC_IFC_COMP0);
}


/**************************************************************************//**
 * @brief RTCC initialization
 *****************************************************************************/
void initRTCC (void)
{
	/* Enable the oscillator for the RTC */
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

	/* Turn on the clock for Low Energy clocks */
	CMU_ClockEnable(cmuClock_HFLE, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);

	/* Turn on the RTC clock */
	CMU_ClockEnable(cmuClock_RTC, true);

	/* Set RTC compare value for RTC compare register 0 */
	RTC_CompareSet(0, COMPARE_RTC);

	/* Allow channel 0 to cause an interrupt */
	RTC_IntEnable(RTC_IEN_COMP0);
	NVIC_ClearPendingIRQ(RTC_IRQn);
	NVIC_EnableIRQ(RTC_IRQn);

	/* Configure the RTC settings */
	RTC_Init_TypeDef rtc = RTC_INIT_DEFAULT;

	/* Initialize RTC with pre-defined settings */
	RTC_Init(&rtc);
}


/**************************************************************************//**
 * @brief
 *   Main function.
 *****************************************************************************/
int main (void)
{
	//uint32_t counter = 0;

	/* Initialize chip */
	CHIP_Init();

	/* Initialize systick */
	if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1);

	/* Initialize RTC compare settings */
	initRTCC();

	/* Initialize USART0 as SPI slave (also initialize CS pin) */
	initADXL_SPI();

	/* Initialize LED's */
	initLEDS();

	/* Initialize GPIO wakeup */
	initWakeup();

#ifdef DEBUGGING /* DEBUGGING */
	dbprint_INIT(USART1, 4, true, false);
#endif /* DEBUGGING */

	/* TODO: this doesn't seem to change anything */
	/* Configure PA8 as an output (EFM_DISP_ENABLE) */
	GPIO_PinModeSet(gpioPortA, 8, gpioModePushPull, 0);

	/* Set EFM_DISP_ENABLE low to disable LCD */
	GPIO_PinOutClear(gpioPortA, 8);



	/* Soft reset ADXL handler */
	resetHandlerADXL();


	/* Profile the ADXL (make sure to not use VCOM here!) */
	//testADXL();


	/* Set the measurement range */
	uint8_t setRange = 1;

	configADXL_range(setRange);

#ifdef DEBUGGING /* DEBUGGING */
	if (setRange == 0) dbinfo("Measurement mode +- 2g selected");
	else if (setRange == 1) dbinfo("Measurement mode +- 4g selected");
	else if (setRange == 2) dbinfo("Measurement mode +- 8g selected");
#endif /* DEBUGGING */


	/* Read and display values forever */
	//readValuesADXL();

	/* Configure activity detection on INT1 */
	configADXL_activity(1); /* 1g */

	/* Enable wake-up mode */
	/* TODO: Fix this */
	//writeADXL(ADXL_REG_POWER_CTL, 0b00001000); /* 5th bit */


	/* Enable measurements */
	writeADXL(ADXL_REG_POWER_CTL, 0b00000010); /* Last 2 bits are measurement mode */

#ifdef DEBUGGING /* DEBUGGING */
	dbinfo("Measurement enabled");
#endif /* DEBUGGING */

	while(1)
	{
		GPIO_PinOutSet(gpioPortF, 4); /* Enable LED0 */
		Delay(1000);
		GPIO_PinOutClear(gpioPortF, 4); /* Disable LED0 */

		/* Read status register to acknowledge interrupt
		 * (can be disabled by changing LINK/LOOP mode in ADXL_REG_ACT_INACT_CTL) */
		if (triggered)
		{
			readADXL(ADXL_REG_STATUS);
			triggered = false;
		}

#ifdef DEBUGGING /* DEBUGGING */
	dbinfo("Disabling systick & going to sleep...\r\n");
#endif /* DEBUGGING */
		disableSystick();
		EMU_EnterEM2(true); /* "true": Save and restore oscillators, clocks and voltage scaling */
		enableSystick();
	}
}
