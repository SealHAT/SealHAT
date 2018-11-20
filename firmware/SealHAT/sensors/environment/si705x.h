#ifndef __SI705X_H__
#define __SI705X_H__

#include "hal_i2c_m_sync.h" // device specific IO functions
#include <stdint.h>

/** @brief initializes the temperature sensor data structures
 *
 * This function initializes the data structures and sets the appropriate ports
 * to use on the device. The address of this device is fixed and so is not a
 * parameter.
 *
 * @param WIRE_I2C [IN] The device structure to use for the sensor (I2C Synchronous)
 * @return ERR_NONE if successful, system error otherwise
 */
int32_t si705x_init(struct i2c_m_sync_desc* const WIRE_I2C);

/** @brief Set the resolution of the temperature sensor
 *
 * This function sets the resolution of the sensor to one of four pre-defined settings
 *     - TEMP_RES_11 -> 11 bits of resolution (.085 degrees, max 416Hz)
 *     - TEMP_RES_12 -> 12 bits of resolution (.043 degrees, max 263Hz)
 *     - TEMP_RES_13 -> 13 bits of resolution (.022 degrees, max 161Hz)
 *     - TEMP_RES_14 -> 14 bits of resolution (.011 degrees, max 92Hz)
 *
 * @param RES [IN] The resolution to set the sensor to
 * @return ERR_NONE if successful, system error otherwise
 */
int32_t si705x_set_resolution(const uint8_t RES);

/** @brief request a measurement from the sensor asynchronously
 *
 * A reading will take approx 7-8 mSec at full resolution and room temperature.
 * This function takes about 100uSec (tested on 12MHz core with 400k I2C bus).
 * The remaining time can be used to run other tasks.
 * The function si705x_measure_asyncGet() must be called later to obtain the reading.
 *
 * @return ERR_NONE if successful, system error otherwise
 */
int32_t si705x_measure_asyncStart(void);

/** @brief Obtain a previously requested asynchronous measurement
 *
 * This function gets a previously requested asynchronous measurement. If the reading is not
 * ready yet (was called too early) this function will block until the reading is ready and read.
 * the amount of time to block can be set with the timeout parameter. A reading should take
 * from 6-8ms, so if this is called after a 5ms delay a timeout value of ~250 works well.
 *
 * @param reading [OUT] pointer to a storage location for the raw reading
 * @param timeout [IN] The number of times to try reading, will always try at least once.
 * @param doCRC [IN] CRC is expensive, so it will only do it if asked.
 * @return ERR_NONE if successful, or system error code. ERR_BAD_DATA for a failed CRC.
 */
int32_t si705x_measure_asyncGet(uint16_t* reading, int32_t timeout, const bool doCRC);

/** @brief Obtain a temperature measurement from the sensor synchronously
 *
 * @param error [OUT] pointer to an int32_t value to catch errors. 0 on success, -1 is comm error, -2 is failed CRC.
 * @return the raw reading from the sensor
 */
int32_t si705x_measure(uint16_t* reading, const bool doCRC);

/** @brief convert a raw reading into degrees Celsius
 *
 * @param READING [IN] raw reading obtained from si507x_measure()
 * @return the temperature in degrees Celsius
 */
static inline float si705x_celsius(const uint16_t READING) { return ((175.72 * READING) / 65536) - 46.85; }

/** @brief Gets the voltage warning bit from the sensor
 *
 * The sensor will set a bit high if the voltage drops below 1.9 volts, but the
 * sensor will cease to operate at 1.8 volts. Therefore if this function returns
 * false then the voltage is between 1.8V and 1.9V and action should be taken.
 *
 * @return non-zero if the voltage is OK, 0 if the voltage is low
 */
int32_t si705x_voltage_ok();

/** @brief Gets the firmware version of the device
 *
 * The device can report it's firmware version. This will be returned as the hex
 * value that the device gives which can be masked with;
 *      - TEMP_FW1 -> Firmware Version 1.0
 *      - TEMP_FW2 -> Firmware Version 2.0
 *
 * @return the hex value of the firmware register.
 */
uint8_t si705x_fw_version();

/** @brief perform a software reset of the device
 *
 * The sensor can be reset using this function. It will be ready to use
 * 15ms (max) from reset, typically 5ms.
 *
 * @return True if successful, false if not. Any error is likely due to I2C
 */
void si705x_reset();

#endif    /* __SI705X_H__ */
