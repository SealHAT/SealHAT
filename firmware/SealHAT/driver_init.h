/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_INCLUDED
#define DRIVER_INIT_INCLUDED

#include "atmel_start_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>

#include <hal_crc_sync.h>
#include <hal_ext_irq.h>

#include <hal_flash.h>

#include <hal_calendar.h>
#include <hal_spi_m_sync.h>

#include <hal_i2c_m_sync.h>

#include <hal_i2c_m_sync.h>

#include <hal_i2c_m_sync.h>
#include <hal_spi_m_sync.h>
#include <tc_lite.h>

#include <hal_wdt.h>

#include "hal_usb_device.h"

extern struct crc_sync_descriptor CRC_0;

extern struct flash_descriptor FLASH_NVM;

extern struct calendar_descriptor   RTC_CALENDAR;
extern struct spi_m_sync_descriptor SPI_MOD;

extern struct i2c_m_sync_desc I2C_GPS;

extern struct i2c_m_sync_desc I2C_ENV;

extern struct i2c_m_sync_desc       I2C_IMU;
extern struct spi_m_sync_descriptor SPI_MEMORY;

extern struct wdt_descriptor WATCHDOG;

void FLASH_NVM_init(void);
void FLASH_NVM_CLOCK_init(void);

void RTC_CALENDAR_CLOCK_init(void);
void RTC_CALENDAR_init(void);

void SPI_MOD_PORT_init(void);
void SPI_MOD_CLOCK_init(void);
void SPI_MOD_init(void);

void I2C_GPS_CLOCK_init(void);
void I2C_GPS_init(void);
void I2C_GPS_PORT_init(void);

void I2C_ENV_CLOCK_init(void);
void I2C_ENV_init(void);
void I2C_ENV_PORT_init(void);

void I2C_IMU_CLOCK_init(void);
void I2C_IMU_init(void);
void I2C_IMU_PORT_init(void);

void SPI_MEMORY_PORT_init(void);
void SPI_MEMORY_CLOCK_init(void);
void SPI_MEMORY_init(void);

void TIMER_MS_CLOCK_init(void);

int8_t TIMER_MS_init(void);

void WATCHDOG_CLOCK_init(void);
void WATCHDOG_init(void);

void USB_DEVICE_INSTANCE_CLOCK_init(void);
void USB_DEVICE_INSTANCE_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED
