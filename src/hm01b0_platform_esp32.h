#ifndef _HM01B0_PLATFORM_ESP32_H_
#define _HM01B0_PLATFORM_ESP32_H_

#include <Arduino.h>
#include "soc/soc.h"
#include "soc/gpio_reg.h"
#include "driver/dedic_gpio.h" // Hardware Acceleration

#ifdef __cplusplus
extern "C" {
#endif
extern dedic_gpio_bundle_handle_t rx_bundle;
#ifdef __cplusplus
}
#endif

// Sync Pins
#define HM01B0_PIN_PCLK  4
#define HM01B0_PIN_HSYNC 5
#define HM01B0_PIN_VSYNC 3

// Safe Data Pins 
//#define D0_PIN 
#define D1_PIN 2
#define D2_PIN 6
#define D3_PIN 15
#define D4_PIN 16
#define D5_PIN 21
#define D6_PIN 47
#define D7_PIN 48

#define HM01B0_READ_PCLK     ((REG_READ(GPIO_IN_REG) >> HM01B0_PIN_PCLK) & 1)
#define HM01B0_READ_HSYNC    ((REG_READ(GPIO_IN_REG) >> HM01B0_PIN_HSYNC) & 1)
#define HM01B0_READ_VSYNC    ((REG_READ(GPIO_IN_REG) >> HM01B0_PIN_VSYNC) & 1)

// 1-CYCLE READ
#define HM01B0_READ_BYTE ((uint8_t)dedic_gpio_bundle_read_in(rx_bundle))

#endif