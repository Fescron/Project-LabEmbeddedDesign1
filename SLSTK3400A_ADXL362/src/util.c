/***************************************************************************//**
 * @file util.c
 * @brief Utility functions.
 * @version 2.0
 * @author Brecht Van Eeckhoudt
 ******************************************************************************/


#include "../inc/util.h"


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
