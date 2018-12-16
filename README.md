# Project-LabEmbeddedDesign1

All the designed code and written reports created for **Lab Embedded Design 1** at *KULeuven Technologiecampus Gent - Master's Electronics Engineering* are gathered here. **Chris Thoen** helped with the research and reports but I mostly focussed on developing the code.

The main code is developed for use on the `Silicion Labs Happy Gecko EFM32 board (EFM32HG322 -- TQFP48)` and was developed on `Simplicity Studio v4` on `Ubuntu 18.04 LTS`. It uses the `ADXL362` accelerometer from *Analog Devices*.

The main project also uses code from **dbprint**, this needs to be added alongside the code on this repository to get the code working. See [dbprint GIT repo](https://github.com/Fescron/dbprint) for more info regarding this.

------

## 1 - GIT repo structure

### 1.1 - code/SLSTK3400A_blink-unoptimized

This is an un-optimized **blink** example which was created at the start of the lab sessions to get to know *Simplicity Studio*. The code created here is referenced in [this report](doc/reports/EmbeddedDesign1-labo1-2-BrechtVanEeckhoudt-ChrisThoen.pdf) (Dutch).

### 1.2 - code/SLSTK3400A_blink-optimized

This is an optimized **blink** example which was created at the start of the lab sessions to get to know *Simplicity Studio*. The code created here is referenced in [this report](doc/reports/EmbeddedDesign1-labo1-2-BrechtVanEeckhoudt-ChrisThoen.pdf) (Dutch).

### 1.3 - code/SLSTK3400A_ADXL362

This is the main code developed for the project for the lab sessions. This is where most of the rest of this *readme* is about.

------

## 2 - Documentation

TODO

### 2.1 - Code flow

TODO (flowchart)

------

## 3 - Development problems

Along the way there were some hiccups in the code-development. The main problem we first faced is discussed in short below.

We first left `automatic ChipSelect` in the `USART config` enabled. After getting no response from the accelerometer we observed the `SPI` bus with a logic analyser. Below we see the incorrect behaviour, normally the accelerometer should perform a **soft reset**. 

![Auto CS = true](/doc/reports/figures/ADXL-reset-autoCStrue.png?raw=true "Auto CS = true")

**As seen above, the ChipSelect pin goes low for each byte. This is not the correct behaviour, since it needs to stay low for three bytes** (`register address`- `read/write` - `value to read/write`).

After manually setting the CS pin high and low we got the correct behaviour, as depicted below.

![Auto CS = false](/doc/reports/figures/ADXL-reset-autoCSfalse-CSPD4.png?raw=true "Auto CS = false")

The same behaviour was observed when we tried to *read a register*. The first picture below is the incorrect behaviour, afterwards we see the accelerometer responding correctly.

![Auto CS = true](/doc/reports/figures/ADXL-read-autoCStrue.png?raw=true "Auto CS = true")

![Auto CS = false](/doc/reports/figures/ADXL-read-autoCSfalse-CSPD4.png?raw=true "Auto CS = false")

------

## 4 - Future

### 4.1 - Wakeup-mode

The accelerometer can be put in a `wakeup-mode` where he only consumes about **270 nA** (@2.0V) and measures the acceleration *about six times per second* to determine whether motion is present or absent. If motion is detected, the accelerometer can respond autonomously in the following ways:
- Switch into full bandwidth measurement mode.
- Signal an interrupt to a microcontroller.
- Wake up downstream circuitry, depending on the configuration.

In wake-up mode, all accelerometer features are available with the exception of the activity timer. All registers can be accessed, and real-time data can be read and/or stored in the FIFO.

**This was not (yet) implemented since it would be very hard to measure these very small current differences. Therefor it would be hard to check if the accelerometer behaves like it should, and the time to get this working would perhaps better be used somewhere else...**

### 4.2 - FIFO and wave frequency

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
