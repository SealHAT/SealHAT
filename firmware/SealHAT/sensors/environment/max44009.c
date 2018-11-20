#include "max44009.h"
#include "max44009_private.h"

#define FULL_ACCURACY_CONSTANT      (0.045)
#define LOW_ACCURACY_CONSTANT       (0.720)

static struct i2c_m_sync_desc max44009_sync;

/* read a single register */
static inline int32_t readReg(const uint8_t REG, uint8_t* data)
{
    return i2c_m_sync_cmd_read(&max44009_sync, REG, data, 1);
}

/* write a single register */
static inline int32_t writeReg(const uint8_t REG, uint8_t val)
{
    return i2c_m_sync_cmd_write(&max44009_sync, REG, &val, 1);
}

int32_t max44009_init(struct i2c_m_sync_desc* const WIRE_I2C, const uint8_t ADDR)
{
    int32_t err;    // return value

    max44009_sync = *WIRE_I2C;

    err = i2c_m_sync_enable(&max44009_sync);
    if(!err) {
        err = i2c_m_sync_set_slaveaddr(&max44009_sync, ADDR, I2C_M_SEVEN);
    }

    return err;
}

int32_t max44009_configure(const uint8_t configuration)
{
    return writeReg(LIGHT_ISR_CONFIG, configuration);
}

int32_t max44009_isr(const uint8_t enable)
{
    return writeReg(LIGHT_ISR_ENABLE, enable);
}

int32_t max44009_read(uint16_t* luxVal)
{
    int32_t err;          // err value catcher
    uint8_t data = 0x00;  // temporary read data

    // reset the return value
    *luxVal = 0x0000;

    err = readReg(LIGHT_LUX_MSB, &data);
    *luxVal |= (data << 4);

    if(!err) {
        err = readReg(LIGHT_LUX_LSB, &data);
        *luxVal |= (data & 0x0F);
    }

    return err;
}

uint32_t max44009_lux_integer(const uint16_t reading)
{
	uint8_t exponent;
	uint8_t mantissa;

	exponent = reading >> 8;
	mantissa = reading & 0xFF;

	return (1<<exponent) * mantissa * FULL_ACCURACY_CONSTANT;
}

float max44009_lux_float(const uint16_t reading)
{
    uint8_t exponent;
    uint8_t mantissa;

    exponent = reading >> 8;
    mantissa = reading & 0xFF;

    return (1<<exponent) * (float)mantissa * FULL_ACCURACY_CONSTANT;
}

bool max44009_set_window(const uint8_t upper, const uint8_t lower, const uint8_t timer)
{
    // TODO
    // upper and lower will need to be calculated since device wants mantissa and exp
    writeReg(LIGHT_UPPER_LIMIT, upper);
    writeReg(LIGHT_LOWER_LIMIT, lower);
    writeReg(LIGHT_TIMER, timer);
    return true;
}
