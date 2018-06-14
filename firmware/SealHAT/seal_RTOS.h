/*
 * seal_RTOS.h
 *
 * Created: 30-Apr-18 23:06:44
 *  Author: Ethan
 */

#ifndef SEAL_RTOS_H_
#define SEAL_RTOS_H_

#define SEAL_DEBUG

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "limits.h"
#include "event_groups.h"
#include "message_buffer.h"

#include "driver_init.h"
#include "atmel_start_pins.h"
#include "hal_rtos.h"

#include "seal_Types.h"
#include "seal_UTIL.h"

#define RTC_PERIODIC_INTERRUPT_SYSTICK  RTC_MODE0_INTFLAG_PER3  // 16 Hz periodic RTC interrupt

#define CONFIG_BLOCK_BASE_ADDR          (0x3F840)   /* First writable page address of on-chip EEPROM. */

// 24-bit system wide event group. NEVER use the numbers directly, they are subject to change. Use the names.
typedef enum {
    // System state alerts
    EVENT_VBUS          = 0x00000001, // indicated the current VBUS level, use USB API to check USB state
    EVENT_LOW_BATTERY   = 0x00000002, // Indicates the battery has reached a critically low level according to settings
    EVENT_LOGTOFLASH    = 0x00000004, // This bit indicates that the system should be logging data to the flash memory
    EVENT_LOGTOUSB      = 0x00000008, // This bit indicates that the device should be streaming data over USB
    EVENT_DEBUG         = 0x00000010, // This bit indicates that the device is in debug mode. this overrides the other modes.
    EVENT_CONFIG_START  = 0x00000020,
    EVENT_RETRIEVE      = 0x00000040,
    EVENT_SYS_6         = 0x00000080,
    EVENT_MASK_SYS      = 0x000000FF, // Mask for watching the system flags

    // IMU events. names assume pin 1 of the IMU is in the upper right
    // Consumers of these flags are responsible for clearing them
    EVENT_MOTION_SHIFT  = 8,            // number of bits to shift the LSM303 motion alerts register to match these bits
    EVENT_MASK_IMU      = 0x0000FF00,   // mask for watching the IMU bits
    EVENT_MASK_IMU_X    = 0x00000300,   // mask for isolating the IMU X axis
    EVENT_MASK_IMU_Y    = 0x00000C00,   // mask for isolating the IMU X axis
    EVENT_MASK_IMU_Z    = 0x00003000,   // mask for isolating the IMU X axis
    EVENT_MOTION_XLOW   = 0x00000100,
    EVENT_MOTION_XHI    = 0x00000200,
    EVENT_MOTION_YLOW   = 0x00000400,
    EVENT_MOTION_YHI    = 0x00000800,
    EVENT_MOTION_ZLOW   = 0x00001000,
    EVENT_MOTION_ZHI    = 0x00002000,
    EVENT_IMU_ACTIVE    = 0x00004000,
    EVENT_IMU_IDLE      = 0x00008000,

    EVENT_FLASH_FULL    = 0x00010000,   // mask for determining if external flash is full
    EVENT_TIME_CHANGE   = 0x00020000,   // indicates the RTC time has changed
    EVENT_TIME_HOUR     = 0x00040000,   // set at the top of every hour  -- TODO consider combining with EVENT_TIME_CHANGE
    EVENT_GPS_COOLDOWN  = 0x00080000,   // set to prevent GPS from responding to an IMU event
    EVENT_UNUSED_11     = 0x00100000,
    EVENT_UNUSED_12     = 0x00200000,
    EVENT_UNUSED_13     = 0x00400000,
    EVENT_UNUSED_14     = 0x00800000,
    EVENT_MASK_ALL      = 0x00FFFFFF    // mask for all bits
} SYSTEM_EVENT_FLAGS_t;

typedef struct __attribute__((__packed__)){
    SENSOR_CONFIGS_t sensorConfigs;
    uint32_t       current_flash_addr;
    uint8_t        current_flash_chip;
} EEPROM_STORAGE_t;

#define STACK_OVERFLOW_DATA_SIZE        (configMAX_TASK_NAME_LEN*3)
typedef struct __attribute__((__packed__)){
    DATA_HEADER_t header;   // data header
    uint8_t       buff[STACK_OVERFLOW_DATA_SIZE];
} STACK_OVERFLOW_PACKET_t;

extern EventGroupHandle_t   xSYSEVENTS_handle;  // event group
extern EEPROM_STORAGE_t     eeprom_data;        //struct containing sensor and SealHAT configurations

void vApplicationIdleHook(void);

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);

/**
 * Function to check the reason for the last reset and deal with it accordingly.
 * Possible Return values:
 *      	Power On Reset         = 1
 *          Brownout Detection 12  = 2
 *          Brownout Detection 33  = 4
 *          External Reset         = 16
 *          Watch Dog Timer        = 32
 *          System Reset Request   = 64
 *          Backup reset           = 128
 * @returns the reset reason enumeration
 */
int32_t checkResetReason(void);

/**
 * This function sets all header fields to zero, and sets the start symbol to the right value.
 * @param header [IN] pointer to a data header struct to initialize
 */
void dataheader_init(DATA_HEADER_t* header);

/**
 * This function fills a header with the current timestamp with seconds and milliseconds.
 * @param header [IN] pointer to a data header struct to fill with the time.
 */
void timestamp_FillHeader(DATA_HEADER_t* header);

/*************************************************************
 * FUNCTION: save_sensor_configs()
 * -----------------------------------------------------------
 * This function writes the SealHAT device's sensor and
 * configuration data out to the chip's onboard EEPROM.
 *************************************************************/
uint32_t eeprom_save_configs(EEPROM_STORAGE_t *config_settings);

/*************************************************************
 * FUNCTION: read_sensor_configs()
 * -----------------------------------------------------------
 * This function reads the SealHAT device's sensor and
 * configuration settings from the onboard EEPROM.
 *************************************************************/
uint32_t eeprom_read_configs(EEPROM_STORAGE_t *config_settings);

#endif /* SEAL_RTOS_H_ */
