/*
 * seal_Types.h
 *
 * Created: 15-May-18 22:45:12
 * Author: Ethan
 */

#ifndef SEAL_TYPES_H_
#define SEAL_TYPES_H_
#include <stdint.h>

// SealHAT hardware version: The format is two digits each for Major.Minor.patch with two digits
// for each number (NO LEADING ZEROS)
//#define SEALHAT_HARDWARE_VERSION    (10040) // Large proto-board with test points
#define SEALHAT_HARDWARE_VERSION    (10060) // small PCB version for final checkoff

// The year to use as the base year. All timestamps will be in seconds since this date, January 1st midnight.
#define SEALHAT_BASE_YEAR           (1970)

#define PAGE_SIZE_EXTRA             (2176)  /* Maximum NAND Flash page size (*including* extra space) */
#define PAGE_SIZE_LESS              (2048)  /* Maximum NAND Flash page size (*excluding* extra space) */

#include "lsm303/LSM303_types.h"
#include "sam-m8q/gps_types.h"
#include "max30003/ecg_types.h"

/** Sensor types */
typedef enum {
    DEVICE_ID_RESERVED          = 0x00,
    DEVICE_ID_ENVIRONMENTAL     = 0x10,
    DEVICE_ID_LIGHT             = 0x20,
    DEVICE_ID_TEMPERATURE       = 0x30,
    DEVICE_ID_ACCELEROMETER     = 0x40,
    DEVICE_ID_MAGNETIC_FIELD    = 0x50,
    DEVICE_ID_GYROSCOPE         = 0x60,
    DEVICE_ID_PRESSURE          = 0x70,
    DEVICE_ID_DEPTH             = 0x80,
    DEVICE_ID_GPS               = 0x90,
    DEVICE_ID_EKG               = 0xA0,
    DEVICE_ID_UNUSED            = 0xB0,
    DEVICE_ID_UNUSED1           = 0xC0,
    DEVICE_ID_SYSTEM            = 0xD0,
    DEVICE_ID_CONFIG            = 0xE0,
    DEVICE_ID_MASK              = 0xF0
} DEVICE_ID_t;

/** Device error codes and flags **/
typedef enum {
    DEVICE_ERR_NONE             = 0x00,
    DEVICE_ERR_COMMUNICATIONS   = 0x01,
    DEVICE_ERR_TIMEOUT          = 0x02,
    DEVICE_ERR_OVERFLOW         = 0x04,
    DEVICE_ERR_MASK             = 0x0F
} DEVICE_ERR_CODES_t;

typedef enum {
    HW_IMU_LSM303AGR            = 0x01,
    HW_IMU_LSM303C              = 0x02,
    HW_LIGHT_SI7051             = 0x03,
    HW_TEMP_MAX44009            = 0x04,
    HW_GPS_SAM_M8Q              = 0x05,
    HW_EKG_MAX30003             = 0x06
} HARDWARE_ID_t;

#define MSG_START_SYM               (0xADDE)
#define MSG_START_SYM_STR           "\xDE\xAD"
#define USB_PACKET_START_SYM        (0x0DD0FECA)
#define USB_PACKET_START_SYM_STR    "\xCA\xFE\xD0\x0D"
#define USB_TEXT_ADVENTURE_MENU     "\n(c)onfigure, (v)erify config, (d)ownload data, (s)tream data, (f)orce Logging, (r)eset\n"

/** Packet that gets sent over USB to the host computer **/
typedef struct __attribute__((__packed__)){
    uint32_t startSymbol;           // start symbol for the data transmission
    uint8_t  data[PAGE_SIZE_LESS];  // one page of data from flash
    uint32_t crc;                   // crc32 of the DATA (not the start symbol) using IEEE CRC32 polynomial
} DATA_TRANSMISSION_t;

typedef enum {
    NO_COMMAND        = 0,
    CONFIGURE_DEV     = 'c',
    VERIFY_CONFIG     = 'v',
    DOWNLOAD_DATA     = 'd',
    STREAM_DATA       = 's',
    FORCE_FLASH       = 'f',
    RESET_SYSTEM      = 'r',
    READY_TO_RECEIVE  = 'a',
    OPERATION_SUCCESS = 'g',
    OPERATION_ERROR   = 'e'
} SYSTEM_COMMANDS;

/** Header for data packets from the device **/
typedef struct __attribute__((__packed__)){
    uint16_t startSym;    // symbol to indicate start of packet
    uint8_t  id;	      // Upper four bits is the device ID, lower four are device specific event flags
    uint8_t  packetCount; // counter to number packets from a specific sensor in leu of milliseconds
    uint32_t timestamp;   // timestamp, seconds since reference year
    uint16_t size;		  // size of data packet to follow in bytes
} DATA_HEADER_t;

typedef struct __attribute__((__packed__)){
    DATA_HEADER_t header;    // packet header
    uint32_t      data[2];   // size of the GP reg in RTC
} SYSTEM_ERROR_t;

#define ENV_LOG_SIZE            (15)
typedef struct __attribute__((__packed__)){
    DATA_HEADER_t header;
    uint16_t      data[ENV_LOG_SIZE];
} ENV_MSG_t;

#define IMU_LOG_SIZE               (25)
typedef struct __attribute__((__packed__)){
    DATA_HEADER_t header;
    AxesRaw_t     data[IMU_LOG_SIZE];
} IMU_MSG_t;

#define GPS_LOG_SIZE               (2)
typedef struct __attribute__((__packed__)) {
    DATA_HEADER_t header;
    gps_log_t     log[GPS_LOG_SIZE];
} GPS_MSG_t;

#define  ECG_LOG_SIZE              (24)
typedef struct __attribute__((__packed__)) {
    DATA_HEADER_t header;
    ECG_SAMPLE_t  log[ECG_LOG_SIZE];
} ECG_MSG_t;

/***********************GUI------------->MICROCONTROLLER*****************/
typedef struct{
   uint32_t         activeHour;     // active hours: the hours this sensor is active
   ACC_FULL_SCALE_t scale;          // full scale reading level (2G, 4G, etc)
   ACC_OPMODE_t     opMode;           // accelerometer mode, sets the power mode and sample rate
   int16_t          threshold;      // threshold of motion to detect in milligravity
   int16_t          duration;       // Duration of movement to detect. not used.
   uint8_t          sensitivity;    // axis to check for motion as defined by ACC_MOTION_AXIS_t
} ACC_CFG_t;

typedef struct{
   uint32_t         activeHour;     // active hours: the hours this sensor is active
   MAG_OPMODE_t     opMode;           // magnetometer mode. sets the rate and power levels
} MAG_CFG_t;


typedef struct{
   uint32_t         activeHour;     // active hours: the hours this sensor is active
   uint16_t         period;         // period of sampling in seconds
} ENV_CFG_t;

typedef struct{
   uint32_t           activeHour;   // active hours: the hours this sensor is active
   uint32_t           activeRate;   // period of sampling while animal is moving in seconds
   uint32_t           idleRate;     // period of sampling while animal is idle in seconds
} GPS_CFG_t;

typedef struct{
   uint32_t           activeHour;   // active hours: the hours this sensor is active
   ECG_SAMPLE_RATE_t  rate;         // sample rate of the ekg
   ECG_GAIN_t         gain;         // gain setting of the ekg
   ECG_LOW_PASS_t     freq;         // low pass cutoff frequency of the ekg
} MOD_CFG_t;

typedef struct __attribute__((__packed__)){
    uint8_t              num_flash_chips;      // number of flash chips installed on device

    // day the device should begin data collection
    uint8_t              start_day;            // range from 1 to 28/29/30/31
    uint8_t              start_month;          // range from 1 to 12
    uint16_t             start_year;           // absolute year >= 1970
    uint8_t              start_hour;           // hour of the day the device will start logging. range 0-23

    ACC_CFG_t            accConfig;           // configuration data for the accelerometer
    MAG_CFG_t            magConfig;           // configuration data for the magnetometer
    ENV_CFG_t            envConfig;           // configuration data for the temperature sensor
    GPS_CFG_t            gpsConfig;           // configuration data for the GPS
    MOD_CFG_t            ekgConfig;           // configuration data for the EKG
} SENSOR_CONFIGS_t;

typedef struct __attribute__((__packed__)){
    DATA_HEADER_t    header;            // packet header
    SENSOR_CONFIGS_t sensorConfigs;     // system configs
    uint32_t         crc32;             // crc32 checksum
} SYSTEM_CONFIG_t;

#endif /* SEAL_TYPES_H_ */
