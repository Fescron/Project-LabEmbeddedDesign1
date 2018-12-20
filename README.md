# Project-LabEmbeddedDesign1

All the designed code and written reports created for **Lab Embedded Design 1** at *KULeuven Technologiecampus Gent - Master's Electronics Engineering* are gathered here. **Chris Thoen** helped with the research and reports but I mostly focussed on developing the code.

The main code is developed for use on the `Silicion Labs Happy Gecko EFM32 board (EFM32HG322 -- TQFP48)` and was developed on `Simplicity Studio v4` on `Ubuntu 18.04 LTS`. It uses the `ADXL362` accelerometer from *Analog Devices* on a [breakout board from Sparkfun](https://www.sparkfun.com/products/11446).

<br/>

The main project also uses code from **dbprint**, this needs to be added alongside the code on this repository to get the code working. See [dbprint GIT repo](https://github.com/Fescron/dbprint) for more info regarding this.

![Project](/doc/reports/figures/project.jpg?raw=true "Project")

------

## 1 - GIT repo structure

### 1.1 - code/SLSTK3400A_blink-unoptimized/

This is an un-optimized **blink** example which was created at the start of the lab sessions to get to know *Simplicity Studio*. The code created here is referenced in [this report](doc/reports/EmbeddedDesign1-labo1-2-BrechtVanEeckhoudt-ChrisThoen.pdf) (Dutch).

### 1.2 - code/SLSTK3400A_blink-optimized/

This is an optimized **blink** example which was created at the start of the lab sessions to get to know *Simplicity Studio*. The code created here is referenced in [this report](doc/reports/EmbeddedDesign1-labo1-2-BrechtVanEeckhoudt-ChrisThoen.pdf) (Dutch).

### 1.3 - code/SLSTK3400A_ADXL362

This is the main code developed for the project for the lab sessions. This is where most of the rest of this *readme* is about.

------

## 2 - Documentation

**The following documentation is all about** `code/SLSTK3400A_ADXL362/`.

<br/>

### 2.1 - File structure

#### 2.1.1 - Header and source files

The corresponding `header` (`.h`) and `source` (`.c`) files are placed in the respective folders `inc` and `src`.

In the **header** files we've put:

- All other includes to other libraries/code we need
- PORT, PIN and REGISTER definitions
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
  
- `debuging.h`
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

## 3 - Development problems

Along the way there were some hiccups in the code-development. The main problem we first faced is discussed in short below.

We first left `automatic ChipSelect` in the `USART config` enabled. After getting no response from the accelerometer we observed the `SPI` bus with a logic analyser. Below we see the incorrect behaviour, normally the accelerometer should perform a **soft reset**. 

![Auto CS = true](/doc/reports/figures/ADXL-reset-autoCStrue.png?raw=true "Auto CS = true")

**As seen above, the ChipSelect pin goes low for each byte. This is not the correct behaviour, since it needs to stay low for three bytes** (`register address`- `read/write` - `value to read/write`).

<br/>

After manually setting the CS pin high and low we got the correct behaviour, as depicted below.

![Auto CS = false](/doc/reports/figures/ADXL-reset-autoCSfalse-CSPD4.png?raw=true "Auto CS = false")

<br/>

The same behaviour was observed when we tried to *read a register*. The first picture below is the incorrect behaviour, afterwards we see the accelerometer responding correctly.

![Auto CS = true](/doc/reports/figures/ADXL-read-autoCStrue.png?raw=true "Auto CS = true")

![Auto CS = false](/doc/reports/figures/ADXL-read-autoCSfalse-CSPD4.png?raw=true "Auto CS = false")

------

## 4 - Future

### 4.1 - Process the data

Right now data doesn't really get read in by the sensor, only the LED is turned on and the interrupt on the accelerometer is cleared if necessary. In the future this could change.

<br/>

### 4.2 - Wakeup-mode

The accelerometer can be put in a `wakeup-mode` where he only consumes about **270 nA** (@2.0V) and measures the acceleration *about six times per second* to determine whether motion is present or absent. If motion is detected, the accelerometer can respond autonomously in the following ways:

- Switch into full bandwidth measurement mode.
- Signal an interrupt to a microcontroller.
- Wake up downstream circuitry, depending on the configuration.

In wake-up mode, all accelerometer features are available with the exception of the activity timer. All registers can be accessed, and real-time data can be read and/or stored in the FIFO.

**This was not (yet) implemented since it would be very hard to measure these very small current differences. Therefor it would be hard to check if the accelerometer behaves like it should, and the time to get this working would perhaps better be used somewhere else...**

<br/>

### 4.3 - FIFO and wave frequency

We can perhaps use the `FIFO` to store measurements at an optimal `ODR` (Output Data Rate) so the **wave frequency** can be calculated using *FFT* functionality available in `CMSIS` libraries. The accelerometer could fill this FIFO on it's own and signal to the microcontroller when it is filled by using an interrupt (there is still one pin unused). Then the microcontroller can read all these values at once and calculate the frequency, after which he again goes to sleep. *We also need to look into the amount of samples we need for this to work.*

------

## 5 - Current measurements

These are rough measurements (16-12-2018), the values were not read from the sensor, it was only put in measurement mode at the given ODR:

- ODR 12,5 HZ ~ 29,60 - 30,41 µA
- ODR 25 Hz ~ 28,43 µA (?)
- ODR 50 Hz ~ 30,88 µA
- ODR 100 Hz ~ 32,09 µA
- ODR 200 Hz ~ 33,77 µA
- ODR 400 Hz ~ 35,43 - 36 µA

**The microcontroller was manually reset each time using the button to get these "correct" values.**

<br/>

**More current measurements can be found at [doc/reports/current-measurements/](/doc/reports/current-measurements)**
