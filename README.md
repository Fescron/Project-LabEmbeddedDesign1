# Project-LabEmbeddedDesign1

All the designed code and written reports created for **Lab Embedded Design 1** at *KULeuven Technologiecampus Gent - Master's Electronics Engineering* are gathered here. **Chris Thoen** helped with the research and reports but I mostly focussed on developing the code.

The main code is developed for use on the `Silicion Labs Happy Gecko EFM32 board (EFM32HG322 -- TQFP48)` and was developed on `Simplicity Studio v4` on `Ubuntu 18.04 LTS`. It uses the `ADXL362` accelerometer from *Analog Devices* on a [breakout board from Sparkfun](https://www.sparkfun.com/products/11446).

<br/>

<!--
The main project also uses code from **dbprint**, this needs to be added alongside the code on this repository to get the code working. See [dbprint GIT repo](https://github.com/Fescron/dbprint) for more info regarding this.
-->

![Project](/doc/reports/figures/project.jpg?raw=true "Project")

------

## Table of contents

- [Project-LabEmbeddedDesign1](#project-labembeddeddesign1)
  - [Table of contents](#table-of-contents)
  - [1 - GIT repo structure](#1---git-repo-structure)
    - [1.1 - code/SLSTK3400A_blink-unoptimized/](#11---codeslstk3400a_blink-unoptimized)
    - [1.2 - code/SLSTK3400A_blink-optimized/](#12---codeslstk3400a_blink-optimized)
    - [1.3 - code/SLSTK3400A_ADXL362](#13---codeslstk3400a_adxl362)
  - [2 - Documentation](#2---documentation)
    - [2.1 - File structure](#21---file-structure)
      - [2.1.1 - Header and source files](#211---header-and-source-files)
      - [2.1.2 - Used files](#212---used-files)
    - [2.2 - Code flow](#22---code-flow)
    - [2.3 - Demo](#23---demo)
  - [3 - Future](#3---future)
    - [3.1 - Process the data](#31---process-the-data)
    - [3.2 - Wakeup-mode](#32---wakeup-mode)
    - [3.3 - FIFO and wave frequency](#33---fifo-and-wave-frequency)

------

## 1 - GIT repo structure

### 1.1 - code/SLSTK3400A_blink-unoptimized/

This is an un-optimized **blink** example which was created at the start of the lab sessions to get to know *Simplicity Studio*. The code created here is referenced in [this report](doc/reports/EmbeddedDesign1-labo1-2-BrechtVanEeckhoudt-ChrisThoen.pdf) (Dutch).

### 1.2 - code/SLSTK3400A_blink-optimized/

This is an optimized **blink** example which was created at the start of the lab sessions to get to know *Simplicity Studio*. The code created here is referenced in [this report](doc/reports/EmbeddedDesign1-labo1-2-BrechtVanEeckhoudt-ChrisThoen.pdf) (Dutch).

### 1.3 - code/SLSTK3400A_ADXL362

This is the main code developed for the project for the lab sessions. This is where the rest of this *readme* is about. This code is also talked about in more detail in [this report](doc/reports/EmbeddedDesign1-Labo-project-BrechtVanEeckhoudt-ChrisThoen.pdf) (Dutch).

> **UPDATE (17-03-2019):** Fixed too high sleep current (>30 µA) caused by the accelerometer sitting on the unused SPI bus. The solution was to disable the SPI pins before going to sleep. This was fixed in the code but the sleep-current-values in the report are wrong. The new sleep current is **less than 4 µA**.

------

## 2 - Documentation

**The following documentation is all about** `code/SLSTK3400A_ADXL362/`.

<br/>

### 2.1 - File structure

#### 2.1.1 - Header and source files

The corresponding `header` (`.h`) and `source` (`.c`) files are placed in the respective folders `inc` and `src`.

In the **header** files we've put:

- All other includes to other libraries/code we need
- REGISTER definitions (PORT and PIN definitions are located in `pin_mapping.h`)
- Public variable declararions
- Method prototypes

In the **source** files we've put:

- A single include to the corresponding header file
- The (public) variable instantiations
- All of the documentation
- The function implementations

<br/>

#### 2.1.2 - Used files

These are all of the files we've created to make the project more *readable*:

- `main.c`
  - Here is the **main method** where everything starts from. We've also added **two initialisation methods** that configure the `EMF32` to **wakeup from pin interrupts** and/or it's **`RTC compare` ability**.

- `pin_mapping.h`
  - Here we put all the PORT and PIN definitions.
  
- `debugging.h`
  - Here we can **enable or disable debugging over UART** by (un)commenting one `#define` line. This file is called in every other file where there are UART (`dbprint`) calls. Because these calls are surrounded by `#ifdef DEBUGGING ... #endif` tags, the statements are included/excluded in the uploaded code.
  
- `util.c` (& `util.h`)
  - Here we have some *utility functionality* like:
    - A **Delay** method with it's **interrupt handler** and a function to **enable or disable** `systicks`.
    - A method to **initialize the LED's** and to **turn on or off LED0**.
    - A method to **stop code execution when an `error` occured** and flash the LED's to indicate this.
  
- `handlers.c` (& `handlers.h`)
  - Here we've gathered the *interrupt handlers* for **`RTC compare`** and **odd and even pin interrupts**.
  
- `accel.c`(& `accel.h`)
  - Here we've gathered all the methods that have something to do with the accelerometer:
    - `void initADXL_VCC (void)`: Initialize and enable the GPIO pin wich powers the accelerometer.
    - `void powerADXL (bool enabled)`: Enable or disable the GPIO pin wich powers the accelerometer.
    - `void enableSPIpinsADXL (bool enabled)`: Enable or disable the SPI pins to the accelerometer.
    - `void initADXL_SPI (void)`: Initialize the SPI pins and settings to communicate with the accelerometer.
    - `void testADXL (void)`: Test all the ODR (Output Data Rate) settings to see the effect they have on power.
    - `void readValuesADXL (void)`: Read and display the X-Y-Z g-values on UART.
    - `void resetHandlerADXL (void)`: This method tries to read the sensor ID. If this fails it tries to *soft reset* it and tries to read the ID again after a delay of one second. If this fails again the power to the sensor is turned off for one second. If the ID check fails again after this, the code stops executing and the `error` method is called.
    - `uint8_t readADXL (uint8_t address)`: Read one byte of data from a given register address. This method is called by other methods like `readADXL_XYZDATA`.
    - `void writeADXL (uint8_t address, uint8_t data)`: Write one byte of data to a given register address. This method is called by other methods like `configADXL_ODR`, `configADXL_range`, ... .
    - `void readADXL_XYZDATA (void)`: Read the X-Y-Z data registers using *burst reads* and put the response data in the global array.
    - `void configADXL_ODR (uint8_t givenODR)`: Configure the Output Data Rate (ODR).
    - `void configADXL_range (uint8_t givenRange)`: Configure the measurement range and store the selected one in a global variable.
    - `void configADXL_activity (uint8_t gThreshold)`: Configure the accelerometer to work in activity threshold mode with a given *g-value*. This way the accelerometer generates an interrupt to wakeup the microcontroller if a value higher than the given threshold is detected.
    - `void measureADXL (bool enabled)`: Enable or disable measurement mode.
    - `void softResetADXL (void)`: Write `'R'` to the *soft reset register* to soft-reset the accelerometer. This method is called by `resetHandlerADXL`.
    - `bool checkID_ADXL (void)`: Check if the ID is correct. This method is called by `resetHandlerADXL`.
    - `int32_t convertGRangeToGValue (int8_t sensorValue)`: Convert sensor readout-value in +-g range to mg value. This method is called by `readADXL_XYZDATA`.

- `dbprint.c` (& `dbprint.h`)
  - Here a lot of debugging methods are implemented. For more info see [dbprint GIT repo](https://github.com/Fescron/dbprint).

<br/>

### 2.2 - Code flow

![Flowchart](/doc/reports/figures/flowchart.png?raw=true "Flowchart")

**Extra notes on the flowchart:**

- **(\*1):** Every minute the RTC compare function will give an interrupt to wakeup the microcontroller.
- **(\*2):** This handler tries to read the sensor ID. If this fails it tries to *soft reset* it and tries to read the ID again after a delay of one second. If this fails again the power to the sensor is turned off for one second. If the ID check fails again after this, the code stops executing and the `error` method is called.
- **(\*3):** Every minute an interrupt gets generated by the Real Time Counter Compare functionality.
- **(\*4):** This can be a button (`PB0` or `PB1`) interrupt or an interrupt generated by the **accelerometer** if the activity threshold is surpassed.

<br/>

### 2.3 - Demo

The following two figures are output examples in the UART console. The first one depicts using the method `readValuesADXL();`, the second one depicts waking up from an interrupt when the threshold is exceeded, a button is pushed or the RTC compare functionality triggered an interrupt.

![UART debugging](/doc/reports/figures/uart1.png?raw=true "UART debugging")

![UART debugging](/doc/reports/figures/uart2.png?raw=true "UART debugging")

------

## 3 - Future

### 3.1 - Process the data

Right now data doesn't really get read in by the sensor, only the LED is turned on and the interrupt on the accelerometer is cleared if necessary. In the future this could change.

<br/>

### 3.2 - Wakeup-mode

The accelerometer can be put in a `wakeup-mode` where he only consumes about **270 nA** (@2.0V) and measures the acceleration *about six times per second* to determine whether motion is present or absent. If motion is detected, the accelerometer can respond autonomously in the following ways:

- Switch into full bandwidth measurement mode.
- Signal an interrupt to a microcontroller.
- Wake up downstream circuitry, depending on the configuration.

In wake-up mode, all accelerometer features are available with the exception of the activity timer. All registers can be accessed, and real-time data can be read and/or stored in the FIFO.

**This was not (yet) implemented since it would be very hard to measure these very small current differences. Therefor it would be hard to check if the accelerometer behaves like it should, and the time to get this working would perhaps better be used somewhere else...**

<br/>

### 3.3 - FIFO and wave frequency

We can perhaps use the `FIFO` to store measurements at an optimal `ODR` (Output Data Rate) so the **wave frequency** can be calculated using *FFT* functionality available in `CMSIS` libraries. The accelerometer could fill this FIFO on it's own and signal to the microcontroller when it is filled by using an interrupt (there is still one pin unused). Then the microcontroller can read all these values at once and calculate the frequency, after which he again goes to sleep. *We also need to look into the amount of samples we need for this to work.*
