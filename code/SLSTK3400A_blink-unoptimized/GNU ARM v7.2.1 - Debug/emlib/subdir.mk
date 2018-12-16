################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4/platform/emlib/src/em_assert.c \
/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4/platform/emlib/src/em_cmu.c \
/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4/platform/emlib/src/em_core.c \
/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4/platform/emlib/src/em_emu.c \
/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4/platform/emlib/src/em_gpio.c 

OBJS += \
./emlib/em_assert.o \
./emlib/em_cmu.o \
./emlib/em_core.o \
./emlib/em_emu.o \
./emlib/em_gpio.o 

C_DEPS += \
./emlib/em_assert.d \
./emlib/em_cmu.d \
./emlib/em_core.d \
./emlib/em_emu.d \
./emlib/em_gpio.d 


# Each subdirectory must supply rules for building sources it contributes
emlib/em_assert.o: /home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4/platform/emlib/src/em_assert.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32HG322F64=1' '-DDEBUG=1' -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//hardware/kit/SLSTK3400A_EFM32HG/config" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/CMSIS/Include" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/emlib/inc" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//hardware/kit/common/bsp" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/Device/SiliconLabs/EFM32HG/Include" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"emlib/em_assert.d" -MT"emlib/em_assert.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

emlib/em_cmu.o: /home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4/platform/emlib/src/em_cmu.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32HG322F64=1' '-DDEBUG=1' -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//hardware/kit/SLSTK3400A_EFM32HG/config" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/CMSIS/Include" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/emlib/inc" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//hardware/kit/common/bsp" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/Device/SiliconLabs/EFM32HG/Include" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"emlib/em_cmu.d" -MT"emlib/em_cmu.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

emlib/em_core.o: /home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4/platform/emlib/src/em_core.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32HG322F64=1' '-DDEBUG=1' -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//hardware/kit/SLSTK3400A_EFM32HG/config" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/CMSIS/Include" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/emlib/inc" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//hardware/kit/common/bsp" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/Device/SiliconLabs/EFM32HG/Include" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"emlib/em_core.d" -MT"emlib/em_core.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

emlib/em_emu.o: /home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4/platform/emlib/src/em_emu.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32HG322F64=1' '-DDEBUG=1' -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//hardware/kit/SLSTK3400A_EFM32HG/config" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/CMSIS/Include" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/emlib/inc" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//hardware/kit/common/bsp" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/Device/SiliconLabs/EFM32HG/Include" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"emlib/em_emu.d" -MT"emlib/em_emu.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

emlib/em_gpio.o: /home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4/platform/emlib/src/em_gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32HG322F64=1' '-DDEBUG=1' -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//hardware/kit/SLSTK3400A_EFM32HG/config" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/CMSIS/Include" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/emlib/inc" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//hardware/kit/common/bsp" -I"/home/brecht/Programs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4//platform/Device/SiliconLabs/EFM32HG/Include" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"emlib/em_gpio.d" -MT"emlib/em_gpio.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


