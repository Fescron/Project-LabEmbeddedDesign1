/***************************************************************************//**
 * @file pin_mapping.h
 * @brief The pin definitions.
 * @version 3.1
 * @author Brecht Van Eeckhoudt
 ******************************************************************************/


/* Include guards prevent multiple inclusions of the same header */
#ifndef _PIN_MAPPING_H_
#define _PIN_MAPPING_H_


/* ADXL SPI GPOI */
#define ADXL_CLK_PORT  gpioPortE
#define ADXL_CLK_PIN   12
#define ADXL_MOSI_PORT gpioPortE
#define ADXL_MOSI_PIN  10
#define ADXL_MISO_PORT gpioPortE
#define ADXL_MISO_PIN  11
#define ADXL_NCS_PORT  gpioPortD 	/* Can't use the US0_CS port (PE13) to manually set/clear CS line */
#define ADXL_NCS_PIN   4

/* Other ADXL GPOI */
#define ADXL_INT1_PORT gpioPortD
#define ADXL_INT1_PIN  7
#define ADXL_VCC_PORT  gpioPortD
#define ADXL_VCC_PIN   5

/* Debug LED's */
#define LED0_PORT gpioPortF
#define LED0_PIN  4
#define LED1_PORT gpioPortF
#define LED1_PIN  5

/* Buttons */
#define PB0_PORT gpioPortC
#define PB0_PIN  9
#define PB1_PORT gpioPortC
#define PB1_PIN  10


#endif /* _PIN_MAPPING_H_ */
