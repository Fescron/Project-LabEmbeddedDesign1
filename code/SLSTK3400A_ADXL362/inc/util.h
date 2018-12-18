/***************************************************************************//**
 * @file util.h
 * @brief Utility functions.
 * @version 3.0
 * @author Brecht Van Eeckhoudt
 ******************************************************************************/


/* Include guards prevent multiple inclusions of the same header */
#ifndef _UTIL_H_
#define _UTIL_H_


#include <stdint.h>    /* (u)intXX_t */
#include <stdbool.h>   /* "bool", "true", "false" */
#include "em_device.h" /* Include necessary MCU-specific header file */
#include "em_gpio.h"   /* General Purpose IO */

#include "../inc/debugging.h" /* Enable or disable printing to UART */


/* PORT and PIN definitoons */
#define LED0_PORT gpioPortF
#define LED0_PIN  4
#define LED1_PORT gpioPortF
#define LED1_PIN  5


/* Prototypes */
void initLEDS (void);
void led0 (bool enabled);
void Error (uint8_t number);
void Delay (uint32_t dlyTicks);
void systickInterrupts (bool enabled);


#endif /* _UTIL_H_ */
