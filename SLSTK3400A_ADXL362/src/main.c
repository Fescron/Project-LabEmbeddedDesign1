/***************************************************************************//**
 * @file main.c
 * @brief The main file for the program to interface to the accelerometer.
 * @version 2.0
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


#include <stdint.h>    /* (u)intXX_t */
#include <stdbool.h>   /* "bool", "true", "false" */
#include "em_device.h" /* Include necessary MCU-specific header file */
#include "em_chip.h"   /* Chip Initialization */
#include "em_cmu.h"    /* Clock management unit */
#include "em_gpio.h"   /* General Purpose IO */
#include "em_usart.h"  /* Universal synchronous/asynchronous receiver/transmitter */
#include "em_emu.h"    /* Energy Management Unit */
#include "em_rtc.h"    /* Real Time Counter (RTC) */

#include "../inc/accel.h"    /* Functions related to the accelerometer */
#include "../inc/util.h"     /* Utility functions */
#include "../inc/handlers.h" /* Interrupt handlers */

#include "../inc/debugging.h" /* Enable or disable printing to UART for debugging */


/* Definitions for RTC compare interrupts */
#define DELAY_RTC 60.0 /* seconds */
#define LFXOFREQ 32768
#define COMPARE_RTC (DELAY_RTC * LFXOFREQ)


/* Definitions for buttons */
#define PB0_PORT gpioPortC
#define PB0_PIN  9
#define PB1_PORT gpioPortC
#define PB1_PIN  10



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
	//GPIO_PinModeSet(gpioPortA, 8, gpioModePushPull, 0);

	/* Set EFM_DISP_ENABLE low to disable LCD */
	//GPIO_PinOutClear(gpioPortA, 8);



	/* Soft reset ADXL handler */
	resetHandlerADXL();


	/* Profile the ADXL (make sure to not use VCOM here!) */
	//testADXL();


	/* Set the measurement range (0 - 1 - 2) */
	configADXL_range(1);

	/* Read and display values forever */
	//readValuesADXL();


	/* Configure activity detection on INT1 */
	configADXL_activity(3); /* [g] */

	/* Enable wake-up mode */
	/* TODO: Fix this */
	//writeADXL(ADXL_REG_POWER_CTL, 0b00001000); /* 5th bit */


	/* Enable measurements */
	measureADXL(true);

#ifdef DEBUGGING /* DEBUGGING */
	dbprintln("");
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
