// Data types used by the 44009 light sensor
#ifndef __SI705X_PRIVATE_H__
#define __SI705X_PRIVATE_H__

/**
 * 7-bit I2C slave address for the SI7051 is static
 * R/W bit is the LSB of the address, ORed with these bits.
 */
typedef enum {
    TEMP_ADDR           = 0x40
} SI705X_ADDR_t;

/**
 * Register addresses for the SI705X temperature sensor
 */
typedef enum {
    TEMP_MEASURE_HOLD   = 0xE3,
    TEMP_MEASURE_NOHOLD = 0xF3,
    TEMP_RESET          = 0xFE,
    TEMP_WRITE_USER1    = 0xE6,
    TEMP_READ_USER1     = 0xE7,
    TEMP_READ_ID_1      = 0xFA,
    TEMP_READ_ID_2      = 0x0F,
    TEMP_READ_ID_3      = 0xFC,
    TEMP_READ_ID_4      = 0xC9,
    TEMP_READ_FW_VER1   = 0x84,
    TEMP_READ_FW_VER2   = 0xB8,
} SI705X_REG_t;

/**
 * Masks used to set the resolution in the user register
 */
typedef enum {
    TEMP_RES_11         = 0x81,
    TEMP_RES_12         = 0x01,
    TEMP_RES_13         = 0x80,
    TEMP_RES_14         = 0x00  /* Default setting */
} SI705X_RESOLUTIONS_t;

/**
 * Masks used to read the voltage status in the user register
 */
typedef enum {
    TEMP_VDD_OK         = 0x00,
    TEMP_VDD_LOW        = 0x40  /* Set when voltage is between 1.8V and 1.9V */
} SI705X_VDD_STATUS_t;

/**
 * Register addresses for the SI705X temperature sensor
 */
typedef enum {
    TEMP_FW1            = 0xFF,
    TEMP_FW2            = 0x20
} SI705X_FW_VERSION_t;

#endif    /* __SI705X_TYPES_H__ */
