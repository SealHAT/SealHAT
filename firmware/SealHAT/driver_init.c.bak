/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>

struct spi_m_sync_descriptor SPI_MEMORY;

struct i2c_m_sync_desc I2C_GPS;

struct i2c_m_sync_desc I2C_ENV;

struct i2c_m_sync_desc I2C_IMU;

typedef enum {
    I2C_STATE_UNKNOWN = 0x00,
    I2C_STATE_IDLE    = 0x01,
    I2C_STATE_OWNER   = 0x02,
    I2C_STATE_BUSY    = 0x03
} I2C_STATE_t;

static void I2C_UNBLOCK_BUS(const uint8_t SDA, const uint8_t SCL)
{
    uint32_t i, count;

	// Set pin SDA direction to input, pull off
	gpio_set_pin_direction(SDA, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(SDA, GPIO_PULL_OFF);
    gpio_set_pin_function(SDA, GPIO_PIN_FUNCTION_OFF);

    for(i = 0, count = 0; i < 50; i++) {
        count += gpio_get_pin_level(SDA);
    }

    if(count < 10) {
        // Set pin SCL direction to output, pull off
        gpio_set_pin_direction(SCL, GPIO_DIRECTION_OUT);
        gpio_set_pin_pull_mode(SCL, GPIO_PULL_OFF);
        gpio_set_pin_function(SCL, GPIO_PIN_FUNCTION_OFF);

        for(i = 0; i <= 32; i++) {
            gpio_toggle_pin_level(SCL);
        }
    }

}

void I2C_GPS_PORT_init(void)
{

	gpio_set_pin_pull_mode(GPS_SDA,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(GPS_SDA, PINMUX_PA16C_SERCOM1_PAD0);

	gpio_set_pin_pull_mode(GPS_SCL,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(GPS_SCL, PINMUX_PA17C_SERCOM1_PAD1);
}

void I2C_GPS_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM1_GCLK_ID_SLOW, CONF_GCLK_SERCOM1_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBCMASK_SERCOM1_bit(MCLK);
}

void I2C_GPS_init(void)
{
	I2C_GPS_CLOCK_init();
	i2c_m_sync_init(&I2C_GPS, SERCOM1);
	I2C_GPS_PORT_init();
}

void I2C_ENV_PORT_init(void)
{

	gpio_set_pin_pull_mode(ENV_SDA,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(ENV_SDA, PINMUX_PA08D_SERCOM2_PAD0);

	gpio_set_pin_pull_mode(ENV_SCL,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(ENV_SCL, PINMUX_PA09D_SERCOM2_PAD1);
}

void I2C_ENV_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM2_GCLK_ID_CORE, CONF_GCLK_SERCOM2_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM2_GCLK_ID_SLOW, CONF_GCLK_SERCOM2_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBCMASK_SERCOM2_bit(MCLK);
}

void I2C_ENV_init(void)
{
	I2C_ENV_CLOCK_init();

	i2c_m_sync_init(&I2C_ENV, SERCOM2);

    // unblock the bus if there was an unexpected reset
    I2C_UNBLOCK_BUS(ENV_SDA, ENV_SCL);

	I2C_ENV_PORT_init();
}

void I2C_IMU_PORT_init(void)
{

	gpio_set_pin_pull_mode(IMU_SDA,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(IMU_SDA, PINMUX_PA22C_SERCOM3_PAD0);

	gpio_set_pin_pull_mode(IMU_SCL,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(IMU_SCL, PINMUX_PA23C_SERCOM3_PAD1);
}

void I2C_IMU_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM3_GCLK_ID_CORE, CONF_GCLK_SERCOM3_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM3_GCLK_ID_SLOW, CONF_GCLK_SERCOM3_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBCMASK_SERCOM3_bit(MCLK);
}

void I2C_IMU_init(void)
{
	I2C_IMU_CLOCK_init();
	i2c_m_sync_init(&I2C_IMU, SERCOM3);

    // unblock the bus if there was an unexpected reset
    I2C_UNBLOCK_BUS(IMU_SDA, IMU_SCL);

    I2C_IMU_PORT_init();
}

void SPI_MEMORY_PORT_init(void)
{

	// Set pin direction to input
	gpio_set_pin_direction(MEM_MISO, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(MEM_MISO,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(MEM_MISO, PINMUX_PA12D_SERCOM4_PAD0);

	// Set pin direction to output
	gpio_set_pin_direction(MEM_SCK, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(MEM_SCK,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(MEM_SCK, PINMUX_PA13D_SERCOM4_PAD1);

	// Set pin direction to output
	gpio_set_pin_direction(MEM_MOSI, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(MEM_MOSI,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(MEM_MOSI, PINMUX_PA15D_SERCOM4_PAD3);
}

void SPI_MEMORY_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM4_GCLK_ID_CORE, CONF_GCLK_SERCOM4_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM4_GCLK_ID_SLOW, CONF_GCLK_SERCOM4_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBCMASK_SERCOM4_bit(MCLK);
}

void SPI_MEMORY_init(void)
{
	SPI_MEMORY_CLOCK_init();
	spi_m_sync_init(&SPI_MEMORY, SERCOM4);
	SPI_MEMORY_PORT_init();
}

void USB_DEVICE_INSTANCE_PORT_init(void)
{

	gpio_set_pin_direction(PA24,
	                       // <y> Pin direction
	                       // <id> pad_direction
	                       // <GPIO_DIRECTION_OFF"> Off
	                       // <GPIO_DIRECTION_IN"> In
	                       // <GPIO_DIRECTION_OUT"> Out
	                       GPIO_DIRECTION_OUT);

	gpio_set_pin_level(PA24,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_pull_mode(PA24,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(PA24,
	                      // <y> Pin function
	                      // <id> pad_function
	                      // <i> Auto : use driver pinmux if signal is imported by driver, else turn off function
	                      // <PINMUX_PA24G_USB_DM"> Auto
	                      // <GPIO_PIN_FUNCTION_OFF"> Off
	                      // <GPIO_PIN_FUNCTION_A"> A
	                      // <GPIO_PIN_FUNCTION_B"> B
	                      // <GPIO_PIN_FUNCTION_C"> C
	                      // <GPIO_PIN_FUNCTION_D"> D
	                      // <GPIO_PIN_FUNCTION_E"> E
	                      // <GPIO_PIN_FUNCTION_F"> F
	                      // <GPIO_PIN_FUNCTION_G"> G
	                      // <GPIO_PIN_FUNCTION_H"> H
	                      // <GPIO_PIN_FUNCTION_I"> I
	                      PINMUX_PA24G_USB_DM);

	gpio_set_pin_direction(PA25,
	                       // <y> Pin direction
	                       // <id> pad_direction
	                       // <GPIO_DIRECTION_OFF"> Off
	                       // <GPIO_DIRECTION_IN"> In
	                       // <GPIO_DIRECTION_OUT"> Out
	                       GPIO_DIRECTION_OUT);

	gpio_set_pin_level(PA25,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_pull_mode(PA25,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(PA25,
	                      // <y> Pin function
	                      // <id> pad_function
	                      // <i> Auto : use driver pinmux if signal is imported by driver, else turn off function
	                      // <PINMUX_PA25G_USB_DP"> Auto
	                      // <GPIO_PIN_FUNCTION_OFF"> Off
	                      // <GPIO_PIN_FUNCTION_A"> A
	                      // <GPIO_PIN_FUNCTION_B"> B
	                      // <GPIO_PIN_FUNCTION_C"> C
	                      // <GPIO_PIN_FUNCTION_D"> D
	                      // <GPIO_PIN_FUNCTION_E"> E
	                      // <GPIO_PIN_FUNCTION_F"> F
	                      // <GPIO_PIN_FUNCTION_G"> G
	                      // <GPIO_PIN_FUNCTION_H"> H
	                      // <GPIO_PIN_FUNCTION_I"> I
	                      PINMUX_PA25G_USB_DP);
}

/* The USB module requires a GCLK_USB of 48 MHz ~ 0.25% clock
 * for low speed and full speed operation. */
#if (CONF_GCLK_USB_FREQUENCY > (48000000 + 48000000 / 400)) || (CONF_GCLK_USB_FREQUENCY < (48000000 - 48000000 / 400))
#warning USB clock should be 48MHz ~ 0.25% clock, check your configuration!
#endif

void USB_DEVICE_INSTANCE_CLOCK_init(void)
{

	hri_gclk_write_PCHCTRL_reg(GCLK, USB_GCLK_ID, CONF_GCLK_USB_SRC | GCLK_PCHCTRL_CHEN);
	hri_mclk_set_AHBMASK_USB_bit(MCLK);
	hri_mclk_set_APBBMASK_USB_bit(MCLK);
}

void USB_DEVICE_INSTANCE_init(void)
{
	USB_DEVICE_INSTANCE_CLOCK_init();
	usb_d_init();
	USB_DEVICE_INSTANCE_PORT_init();
}

void system_init(void)
{
	init_mcu();

	// GPIO on PA02

	// Disable digital pin circuitry
	gpio_set_pin_direction(BATTERY_LEVEL, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(BATTERY_LEVEL, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA10

	// Set pin direction to input
	gpio_set_pin_direction(ENV_IRQ, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(ENV_IRQ,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(ENV_IRQ, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA11

	// Set pin direction to input
	gpio_set_pin_direction(VBUS_DETECT, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(VBUS_DETECT,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_DOWN);

	gpio_set_pin_function(VBUS_DETECT, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA14

	// Set pin direction to output
	gpio_set_pin_direction(MEM_CS2, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(MEM_CS2,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(MEM_CS2, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA18

	// Set pin direction to input
	gpio_set_pin_direction(GPS_TIMEPULSE, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(GPS_TIMEPULSE,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(GPS_TIMEPULSE, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA19

	// Set pin direction to input
	gpio_set_pin_direction(GPS_TXD, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(GPS_TXD,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(GPS_TXD, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA20

	// Set pin direction to input
	gpio_set_pin_direction(IMU_INT1_XL, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(IMU_INT1_XL,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(IMU_INT1_XL, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA21

	// Set pin direction to input
	gpio_set_pin_direction(IMU_INT2_XL, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(IMU_INT2_XL,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(IMU_INT2_XL, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA27

	// Set pin direction to input
	gpio_set_pin_direction(IMU_INT_MAG, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(IMU_INT_MAG,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(IMU_INT_MAG, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB02

	// Set pin direction to output
	gpio_set_pin_direction(LED_GREEN, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(LED_GREEN,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	gpio_set_pin_function(LED_GREEN, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB03

	// Set pin direction to output
	gpio_set_pin_direction(LED_RED, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(LED_RED,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	gpio_set_pin_function(LED_RED, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB10

	// Set pin direction to output
	gpio_set_pin_direction(MEM_CS0, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(MEM_CS0,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(MEM_CS0, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB11

	// Set pin direction to output
	gpio_set_pin_direction(MEM_CS1, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(MEM_CS1,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(MEM_CS1, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB22

	// Set pin direction to output
	gpio_set_pin_direction(GPS_EXT_INT, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(GPS_EXT_INT,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(GPS_EXT_INT, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB23

	// Set pin direction to output
	gpio_set_pin_direction(GPS_RESET, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(GPS_RESET,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(GPS_RESET, GPIO_PIN_FUNCTION_OFF);

	I2C_GPS_init();

	I2C_ENV_init();

	I2C_IMU_init();

	SPI_MEMORY_init();

	USB_DEVICE_INSTANCE_init();
}
