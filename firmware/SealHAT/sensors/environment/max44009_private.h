// Data types used by the 44009 light sensor
#ifndef __MAX44009_PRIVATE_H__
#define __MAX44009_PRIVATE_H__

/**
 * Register addresses for the MAX44009 light sensor
 */
typedef enum {
    LIGHT_ISR_STATUS         = 0x00,
    LIGHT_ISR_ENABLE         = 0x01,
    LIGHT_ISR_CONFIG         = 0x02,
    LIGHT_LUX_MSB            = 0x03,
    LIGHT_LUX_LSB            = 0x04,
    LIGHT_UPPER_LIMIT        = 0x05,
    LIGHT_LOWER_LIMIT        = 0x06,
    LIGHT_TIMER              = 0x07
} LIGHT_REG_t;

typedef enum {
    LIGHT_ISR_OFF            = 0x00,
    LIGHT_ISR_ON             = 0x01
} LIGHT_ISR_ENABLED_t;

typedef enum {
    LIGHT_ISR_INACTIVE       = 0x00,
    LIGHT_ISR_ACTIVE         = 0x01
} LIGHT_ISR_TRIGGERED_t;

/********* CONFIGURATION REGISTER MASKS *********/

typedef enum {
    LIGHT_DEFAULT_MODE       = 0x00, /* Measurements every 800ms reguardless of integration time */
    LIGHT_CONTINOUS_MODE     = 0x80  /* new measurements starts as soon as old one ends*/
} LIGHT_CONTINOUS_MODE_t;

typedef enum {
    LIGHT_INTEGRATION_AUTO   = 0x00,  /* Automatic integration time */
    LIGHT_INTEGRATION_MANUAL = 0x40   /* Manually set integration time */
} LIGHT_INTEGRATION_MODE_t;

typedef enum {
    LIGHT_CURRENT_NO_DIV     = 0x00,
    LIGHT_CURRENT_DIVIDE     = 0x08
} LIGHT_CURRENT_DIVISION_t;

typedef enum {
    LIGHT_TIME_800_MS        = 0x00, /* perferred for low light */
    LIGHT_TIME_400_MS        = 0x01,
    LIGHT_TIME_200_MS        = 0x02,
    LIGHT_TIME_100_MS        = 0x03, /* perferred for high-light */
    LIGHT_TIME_50_MS         = 0x04, /* Manual Mode Only */
    LIGHT_TIME_25_MS         = 0x05, /* Manual Mode Only */
    LIGHT_TIME_12_5_MS       = 0x06, /* Manual Mode Only */
    LIGHT_TIME_6_25_MS       = 0x07, /* Manual Mode Only */
} LIGHT_INTEGRATION_TIME_t;

#endif    /* __MAX44009_TYPES_H__ */