/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

/* CRC Data in flash */
COMPILER_ALIGNED(4)
static const uint32_t crc_datas[] = {0x00000000,
                                     0x11111111,
                                     0x22222222,
                                     0x33333333,
                                     0x44444444,
                                     0x55555555,
                                     0x66666666,
                                     0x77777777,
                                     0x88888888,
                                     0x99999999};

/**
 * Example of using CRC_0 to Calculate CRC32 for a buffer.
 */
void CRC_0_example(void)
{
	/* The initial value used for the CRC32 calculation usually be 0xFFFFFFFF,
	 * but can be, for example, the result of a previous CRC32 calculation if
	 * generating a common CRC32 of separate memory blocks.
	 */
	uint32_t crc = 0xFFFFFFFF;
	uint32_t crc2;
	uint32_t ind;

	crc_sync_enable(&CRC_0);
	crc_sync_crc32(&CRC_0, (uint32_t *)crc_datas, 10, &crc);

	/* The read value must be complemented to match standard CRC32
	 * implementations or kept non-inverted if used as starting point for
	 * subsequent CRC32 calculations.
	 */
	crc ^= 0xFFFFFFFF;

	/* Calculate the same data with subsequent CRC32 calculations, the result
	 * should be same as previous way.
	 */
	crc2 = 0xFFFFFFFF;
	for (ind = 0; ind < 10; ind++) {
		crc_sync_crc32(&CRC_0, (uint32_t *)&crc_datas[ind], 1, &crc2);
	}
	crc2 ^= 0xFFFFFFFF;

	/* The calculate result should be same. */
	while (crc != crc2)
		;
}

static void button_on_PA19_pressed(void)
{
}

static void button_on_PA20_pressed(void)
{
}

static void button_on_PA21_pressed(void)
{
}

static void button_on_PA11_pressed(void)
{
}

static void button_on_PA27_pressed(void)
{
}

/**
 * Example of using EXTERNAL_IRQ
 */
void EXTERNAL_IRQ_example(void)
{

	ext_irq_register(PIN_PA19, button_on_PA19_pressed);
	ext_irq_register(PIN_PA20, button_on_PA20_pressed);
	ext_irq_register(PIN_PA21, button_on_PA21_pressed);
	ext_irq_register(PIN_PA11, button_on_PA11_pressed);
	ext_irq_register(PIN_PA27, button_on_PA27_pressed);
}

static uint8_t src_data[128];
static uint8_t chk_data[128];
/**
 * Example of using FLASH_NVM to read and write buffer.
 */
void FLASH_NVM_example(void)
{
	uint32_t page_size;
	uint16_t i;

	/* Init source data */
	page_size = flash_get_page_size(&FLASH_NVM);

	for (i = 0; i < page_size; i++) {
		src_data[i] = i;
	}

	/* Write data to flash */
	flash_write(&FLASH_NVM, 0x3200, src_data, page_size);

	/* Read data from flash */
	flash_read(&FLASH_NVM, 0x3200, chk_data, page_size);
}

/**
 * Example of using RTC_CALENDAR.
 */
static struct calendar_alarm alarm;

static void alarm_cb(struct calendar_descriptor *const descr)
{
	/* alarm expired */
}

void RTC_CALENDAR_example(void)
{
	struct calendar_date date;
	struct calendar_time time;

	calendar_enable(&RTC_CALENDAR);

	date.year  = 2000;
	date.month = 12;
	date.day   = 31;

	time.hour = 12;
	time.min  = 59;
	time.sec  = 59;

	calendar_set_date(&RTC_CALENDAR, &date);
	calendar_set_time(&RTC_CALENDAR, &time);

	alarm.cal_alarm.datetime.time.sec = 4;
	alarm.cal_alarm.option            = CALENDAR_ALARM_MATCH_SEC;
	alarm.cal_alarm.mode              = REPEAT;

	calendar_set_alarm(&RTC_CALENDAR, &alarm, alarm_cb);
}

/**
 * Example of using SPI_MOD to write "Hello World" using the IO abstraction.
 */
static uint8_t example_SPI_MOD[12] = "Hello World!";

void SPI_MOD_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_MOD, &io);

	spi_m_sync_enable(&SPI_MOD);
	io_write(io, example_SPI_MOD, 12);
}

void I2C_GPS_example(void)
{
	struct io_descriptor *I2C_GPS_io;

	i2c_m_sync_get_io_descriptor(&I2C_GPS, &I2C_GPS_io);
	i2c_m_sync_enable(&I2C_GPS);
	i2c_m_sync_set_slaveaddr(&I2C_GPS, 0x12, I2C_M_SEVEN);
	io_write(I2C_GPS_io, (uint8_t *)"Hello World!", 12);
}

void I2C_ENV_example(void)
{
	struct io_descriptor *I2C_ENV_io;

	i2c_m_sync_get_io_descriptor(&I2C_ENV, &I2C_ENV_io);
	i2c_m_sync_enable(&I2C_ENV);
	i2c_m_sync_set_slaveaddr(&I2C_ENV, 0x12, I2C_M_SEVEN);
	io_write(I2C_ENV_io, (uint8_t *)"Hello World!", 12);
}

void I2C_IMU_example(void)
{
	struct io_descriptor *I2C_IMU_io;

	i2c_m_sync_get_io_descriptor(&I2C_IMU, &I2C_IMU_io);
	i2c_m_sync_enable(&I2C_IMU);
	i2c_m_sync_set_slaveaddr(&I2C_IMU, 0x12, I2C_M_SEVEN);
	io_write(I2C_IMU_io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using SPI_MEMORY to write "Hello World" using the IO abstraction.
 */
static uint8_t example_SPI_MEMORY[12] = "Hello World!";

void SPI_MEMORY_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_MEMORY, &io);

	spi_m_sync_enable(&SPI_MEMORY);
	io_write(io, example_SPI_MEMORY, 12);
}

/**
 * Example of using WATCHDOG.
 */
void WATCHDOG_example(void)
{
	uint32_t clk_rate;
	uint16_t timeout_period;

	clk_rate       = 1000;
	timeout_period = 4096;
	wdt_set_timeout_period(&WATCHDOG, clk_rate, timeout_period);
	wdt_enable(&WATCHDOG);
}
