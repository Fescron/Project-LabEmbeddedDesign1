/***************************************************************************//**
 * @file
 * @brief Simple LED Blink Demo for SLSTK3400A_EFM32HG
 * @version 5.6.1
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdint.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_rtc.h"
#include "bsp.h"

#define DELAY_1SEC 1.0
#define DELAY_100MSEC 0.1
#define LFXOFREQ      32768
#define COMPARE_1SEC   (DELAY_1SEC * LFXOFREQ)
#define COMPARE_100MSEC   (DELAY_100MSEC * LFXOFREQ)

uint32_t ledOn = 0;

/**************************************************************************//**
 * @brief RTCC interrupt service routine
 *****************************************************************************/
void RTC_IRQHandler(void)
{
  // If the led is ON: Turn it off & set the RTC compare value to 1 s
  if (ledOn == 0) {
	  ledOn = 1;

	  // Set RTC compare value for RTC 0
	  RTC_CompareSet(0, COMPARE_1SEC);
  }

  // If the led is OFF: Turn it on & set the RTC compare value to 100 ms
  else {
	  ledOn = 0;

	  // Set RTC compare value for RTC 0
	  RTC_CompareSet(0, COMPARE_100MSEC);
  }

  // Reset counter
  RTC_CounterReset();

  // Clear the interrupt source
  RTC_IntClear(RTC_IFC_COMP0);

  // Toggle LED 0
  BSP_LedToggle(0);
}

/**************************************************************************//**
 * @brief RTCC initialization
 *****************************************************************************/
void rtcSetup(void)
{
  // Enable the oscillator for the RTC
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  // Turn on the clock for Low Energy clocks
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);

  // Turn on the RTC clock
  CMU_ClockEnable(cmuClock_RTC, true);

  // Set RTC compare value for RTC 0
  RTC_CompareSet(0, COMPARE_1SEC);

  // Allow channel 0 to cause an interrupt
  RTC_IntEnable(RTC_IEN_COMP0);
  NVIC_ClearPendingIRQ(RTC_IRQn);
  NVIC_EnableIRQ(RTC_IRQn);

  // Configure the RTC settings
  RTC_Init_TypeDef rtc = RTC_INIT_DEFAULT;

  // Initialize RTC with pre-defined settings
  RTC_Init(&rtc);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initializations
  BSP_LedsInit();
  BSP_LedSet(0);

  rtcSetup();

  // Infinite loop
  while(1)
  {
    EMU_EnterEM2(true); // "true": Save and restore oscillators, clocks and voltage scaling
  }
}
