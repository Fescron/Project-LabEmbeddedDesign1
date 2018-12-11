/***************************************************************************//**
 * @file util.h
 * @brief Utility functions.
 * @version 2.0
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


/* Prototypes */
void initLEDS (void);
void Error (uint8_t number);
void Delay (uint32_t dlyTicks);  /* TODO before: Static so the function is only "seen" in the file it's declared in. */
void disableSystick (void);
void enableSystick (void);


#endif /* _UTIL_H_ */
