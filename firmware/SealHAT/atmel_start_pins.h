/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

#define SEALHAT_HARDWARE_VERSION 10060

// SAML21 has 9 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7
#define GPIO_PIN_FUNCTION_I 8

#ifndef SEALHAT_HARDWARE_VERSION
    #error "SEALHAT HARDWARE VERSION MUST BE DEFINED!"
#endif

#if SEALHAT_HARDWARE_VERSION == 10040

    #define XIN32           GPIO(GPIO_PORTA, 0)
    #define XOUT32          GPIO(GPIO_PORTA, 1)
    #define BATTERY_LEVEL   GPIO(GPIO_PORTA, 2)
    #define VBUS_DETECT     GPIO(GPIO_PORTA, 11)
    #define USB_DM          GPIO(GPIO_PORTA, 24)
    #define USB_DP          GPIO(GPIO_PORTA, 25)
    #define LED_RED         GPIO(GPIO_PORTB, 3)
    #define LED_GREEN       GPIO(GPIO_PORTB, 2)
    #define LED_BLUE        LED_GREEN               // NO Blue LED on 1.0.4, this will make firwmware compatable

    #define MOD_MISO        GPIO(GPIO_PORTA, 4)
    #define MOD_SCK         GPIO(GPIO_PORTA, 5)
    #define MOD_CS          GPIO(GPIO_PORTA, 6)
    #define MOD_MOSI        GPIO(GPIO_PORTA, 7)
    #define MOD9            GPIO(GPIO_PORTB, 8)
    #define MOD2            GPIO(GPIO_PORTB, 9)
    #define MOD8            GPIO(GPIO_PORTA, 3)

    #define ENV_SDA         GPIO(GPIO_PORTA, 8)
    #define ENV_SCL         GPIO(GPIO_PORTA, 9)
    #define ENV_IRQ         GPIO(GPIO_PORTA, 10)

    #define MEM_MOSI        GPIO(GPIO_PORTA, 15)
    #define MEM_MISO        GPIO(GPIO_PORTA, 12)
    #define MEM_SCK         GPIO(GPIO_PORTA, 13)
    #define MEM_CS0         GPIO(GPIO_PORTB, 10)
    #define MEM_CS1         GPIO(GPIO_PORTB, 11)
    #define MEM_CS2         GPIO(GPIO_PORTA, 14)

    #define GPS_SDA         GPIO(GPIO_PORTA, 16)
    #define GPS_SCL         GPIO(GPIO_PORTA, 17)
    #define GPS_TXD         GPIO(GPIO_PORTA, 19)
    #define GPS_TIMEPULSE   GPIO(GPIO_PORTA, 18)
    #define GPS_EXT_INT     GPIO(GPIO_PORTB, 22)
    #define GPS_RESET       GPIO(GPIO_PORTB, 23)

    #define IMU_SDA         GPIO(GPIO_PORTA, 22)
    #define IMU_SCL         GPIO(GPIO_PORTA, 23)
    #define IMU_INT1_XL     GPIO(GPIO_PORTA, 20)
    #define IMU_INT2_XL     GPIO(GPIO_PORTA, 21)
    #define IMU_INT_MAG     GPIO(GPIO_PORTA, 27)

#elif SEALHAT_HARDWARE_VERSION == 10060

    #define XIN32           GPIO(GPIO_PORTA, 0)
    #define XOUT32          GPIO(GPIO_PORTA, 1)
    #define BATTERY_LEVEL   GPIO(GPIO_PORTA, 2)
    #define VBUS_DETECT     GPIO(GPIO_PORTA, 18)
    #define USB_DM          GPIO(GPIO_PORTA, 24)
    #define USB_DP          GPIO(GPIO_PORTA, 25)
    #define LED_RED         GPIO(GPIO_PORTB, 23)
    #define LED_GREEN       GPIO(GPIO_PORTB, 2)
    #define LED_BLUE        GPIO(GPIO_PORTB, 3)

    #define MOD_MISO        GPIO(GPIO_PORTA, 4)
    #define MOD_SCK         GPIO(GPIO_PORTA, 5)
    #define MOD_CS          GPIO(GPIO_PORTA, 6)
    #define MOD_MOSI        GPIO(GPIO_PORTA, 7)
    #define MOD_VREF        GPIO(GPIO_PORTA, 3)
    #define MOD1            GPIO(GPIO_PORTB, 8)
    #define MOD2            GPIO(GPIO_PORTB, 9)
    #define MOD3            GPIO(GPIO_PORTA, 10)

    // legacy pins from 104
    #define MOD9            GPIO(GPIO_PORTB, 8)
    #define MOD2            GPIO(GPIO_PORTB, 9)
    #define MOD8            GPIO(GPIO_PORTA, 3)

    #define ENV_SDA         GPIO(GPIO_PORTA, 8)
    #define ENV_SCL         GPIO(GPIO_PORTA, 9)

    #define MEM_MOSI        GPIO(GPIO_PORTA, 15)
    #define MEM_MISO        GPIO(GPIO_PORTA, 12)
    #define MEM_SCK         GPIO(GPIO_PORTA, 13)
    #define MEM_CS0         GPIO(GPIO_PORTB, 10)
    #define MEM_CS1         GPIO(GPIO_PORTB, 11)
    #define MEM_CS2         GPIO(GPIO_PORTA, 14)
    #define MEM_CS3         GPIO(GPIO_PORTA, 11)

    #define GPS_SDA         GPIO(GPIO_PORTA, 16)
    #define GPS_SCL         GPIO(GPIO_PORTA, 17)
    #define GPS_TXD         GPIO(GPIO_PORTA, 19)
    #define GPS_EXT_INT     GPIO(GPIO_PORTB, 22)

    #define IMU_SDA         GPIO(GPIO_PORTA, 22)
    #define IMU_SCL         GPIO(GPIO_PORTA, 23)
    #define IMU_INT1_XL     GPIO(GPIO_PORTA, 20)
    #define IMU_INT2_XL     GPIO(GPIO_PORTA, 21)
    #define IMU_INT_MAG     GPIO(GPIO_PORTA, 27)

#endif

#endif // ATMEL_START_PINS_H_INCLUDED
