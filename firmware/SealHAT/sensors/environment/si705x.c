#include "si705x.h"
#include "si705x_private.h"

static struct i2c_m_sync_desc si705x_sync;

/* read a single register */
static inline int32_t readReg(const uint8_t REG, uint8_t* data)
{
    return i2c_m_sync_cmd_read(&si705x_sync, REG, data, 1);
}

/* write a single register */
static inline int32_t writeReg(const uint8_t REG, uint8_t val)
{
    return i2c_m_sync_cmd_write(&si705x_sync, REG, &val, 1);
}

int32_t si705x_init(struct i2c_m_sync_desc* const WIRE_I2C)
{
    int32_t err;        // err return value

	si705x_sync = *WIRE_I2C;
	err = i2c_m_sync_enable(&si705x_sync);

    if(!err) {
	    err = i2c_m_sync_set_slaveaddr(&si705x_sync, TEMP_ADDR, I2C_M_SEVEN);
    }
    return err;
}

int32_t si705x_set_resolution(const SI705X_RESOLUTIONS_t RES)
{
	return writeReg(TEMP_WRITE_USER1, RES);
}

static uint8_t bitswap(uint8_t input){
    unsigned char output = 0;
    for (int k = 0; k < 8; k++) {
        output |= ((input >> k) & 0x01) << (7 - k);
    }
    return output;
}

static uint8_t crc8(void* inData, uint8_t len, uint8_t init){
    uint8_t* data = (uint8_t*)inData;
    uint8_t crc = bitswap(init);

    for (int i=0; i<len;i++)
    {
        uint8_t inbyte = bitswap(data[i]);
        for (uint8_t j=0;j<8;j++)
        {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix)
            crc ^= 0x8C;

            inbyte >>= 1;
        }
    }
    return bitswap(crc);
}

int32_t si705x_measure_asyncStart(void)
{
    struct _i2c_m_msg msg;
    uint8_t	Reg = TEMP_MEASURE_NOHOLD;

    msg.addr   = si705x_sync.slave_addr;
    msg.len    = 1;
    msg.flags  = I2C_M_STOP;
    msg.buffer = &Reg;

    return _i2c_m_sync_transfer(&si705x_sync.device, &msg);
}

int32_t si705x_measure_asyncGet(uint16_t* reading, int32_t timeout, const bool doCRC)
{
    struct _i2c_m_msg msg;      // I2C transfer struct
    int32_t	err;                // error return value
    uint8_t buf[3];             // data buffer

    msg.addr   = si705x_sync.slave_addr;
    msg.flags  = I2C_M_STOP | I2C_M_RD;
    msg.buffer = buf;
    msg.len    = 3;     // two for temp, 3 for temp plus checksum

    do{
        err = _i2c_m_sync_transfer(&si705x_sync.device, &msg);
    } while (err != ERR_NONE && timeout-- > 0);

    // CRC is expensive, so only do it if requested and no other errors.
    if(!err && doCRC) {
        if( !(crc8(buf, 2, 0x00) == buf[2]) ) {
            err = ERR_BAD_DATA;
        }
    }

    *reading = (buf[0] << 8) | buf[1];

    return err;
}

int32_t si705x_measure(uint16_t* reading, const bool doCRC)
{
	int32_t err = si705x_measure_asyncStart();

    if(!err) {
        err = si705x_measure_asyncGet(reading, 2500, doCRC);
    }

    return err;
}

int32_t si705x_voltage_ok()
{
    int32_t err;    // error return values
	uint8_t data;   // data to get from I2C

    err = readReg(TEMP_READ_USER1, &data);

    return (err != ERR_NONE || (data & TEMP_VDD_LOW));
}

uint8_t si705x_fw_version()
{
    uint8_t fwVer;  // data to get from I2C

    readReg(TEMP_READ_FW_VER1, &fwVer);

    if(TEMP_FW1 != fwVer) {
        readReg(TEMP_READ_FW_VER2, &fwVer);
        if(TEMP_FW2 != fwVer) {
            fwVer = 0;
        }
    }

    return fwVer;
}

void si705x_reset()
{
	writeReg(TEMP_WRITE_USER1, TEMP_RESET);
}
