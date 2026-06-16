/*
 *******************************************************************************
 * Copyright (c) 2023, AirM2M
 * All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */
#pragma once

#define RCC_PLL_SUPPORT

#define PA0                     PIN_A0
#define PA1                     PIN_A1
#define PA2                     PIN_A2
#define PA3                     PIN_A3
#define PA4                     PIN_A4
#define PA5                     PIN_A5
#define PA6                     PIN_A6
#define PA7                     PIN_A7
#define PA8                     8
#define PA9                     9
#define PA10                    10
#define PA11                    11
#define PA12                    12
#define PA13                    13
#define PA14                    14
#define PA15                    15
#define PB0                     PIN_A8
#define PB1                     PIN_A9
#define PB2                     18
#define PB3                     19
#define PB4                     20
#define PB5                     21
#define PB6                     22
#define PB7                     23
#define PB8                     24
#define PB9                     25
#define PB10                    26
#define PB11                    27
#define PB12                    28
#define PB13                    29
#define PB14                    30
#define PB15                    31
#define PF0                     32
#define PF1                     33
#define PF2                     34
#define PF5                     35
#define PF6                     36
#define PF8                     37
#define PF9                     38
#define PC13                    39
#define PC14                    40
#define PC15                    41

#define NUM_DIGITAL_PINS        42
#define NUM_ANALOG_INPUTS       10

#ifndef LED_BUILTIN
  #define LED_BUILTIN           PNUM_NOT_DEFINED
#endif

#ifndef USER_BTN
  #define USER_BTN              PNUM_NOT_DEFINED
#endif

#ifndef PIN_SPI_SS
  #define PIN_SPI_SS            PA4
#endif
#ifndef PIN_SPI_SS1
  #define PIN_SPI_SS1           PA15
#endif
#ifndef PIN_SPI_SS2
  #define PIN_SPI_SS2           PNUM_NOT_DEFINED
#endif
#ifndef PIN_SPI_SS3
  #define PIN_SPI_SS3           PNUM_NOT_DEFINED
#endif
#ifndef PIN_SPI_MOSI
  #define PIN_SPI_MOSI          PA7
#endif
#ifndef PIN_SPI_MISO
  #define PIN_SPI_MISO          PA6
#endif
#ifndef PIN_SPI_SCK
  #define PIN_SPI_SCK           PA5
#endif

#ifndef PIN_WIRE_SDA
  #define PIN_WIRE_SDA          PB7
#endif
#ifndef PIN_WIRE_SCL
  #define PIN_WIRE_SCL          PB6
#endif

#ifndef TIMER_TONE
  #define TIMER_TONE            TIM16
#endif
#ifndef TIMER_SERVO
  #define TIMER_SERVO           TIM14
#endif

#ifndef SERIAL_UART_INSTANCE
  #define SERIAL_UART_INSTANCE  1
#endif
#ifndef PIN_SERIAL_RX
  #define PIN_SERIAL_RX         PA10
#endif
#ifndef PIN_SERIAL_TX
  #define PIN_SERIAL_TX         PA9
#endif

#ifdef VDD_VALUE
#undef VDD_VALUE
#endif
#if defined(VDD_1V8)
#define VDD_VALUE 1800U
#elif defined(VDD_2V5)
#define VDD_VALUE 2500U
#elif defined(VDD_2V8)
#define VDD_VALUE 2800U
#elif defined(VDD_3V3)
#define VDD_VALUE 3300U
#elif defined(VDD_3V8)
#define VDD_VALUE 3800U
#elif defined(VDD_5V)
#define VDD_VALUE 5000U
#else
#error "VDD_VALUE is not defined"
#endif

#ifdef __cplusplus
  #ifndef SERIAL_PORT_MONITOR
    #define SERIAL_PORT_MONITOR   Serial
  #endif
  #ifndef SERIAL_PORT_HARDWARE
    #define SERIAL_PORT_HARDWARE  Serial
  #endif
#endif
