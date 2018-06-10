/*
 * seal_SERIAL.h
 *
 * Created: 27-May-18 2:36:09 PM
 *  Author: Krystine
 */

#ifndef SEAL_SERIAL_H_
#define SEAL_SERIAL_H_

#include "seal_RTOS.h"
#include "seal_Types.h"

#define SERIAL_STACK_SIZE               (1000 / sizeof(portSTACK_TYPE))
#define SERIAL_TASK_PRI                 (tskIDLE_PRIORITY)

extern TaskHandle_t xSERIAL_th;                       // Message accumulator for USB/MEM

typedef enum {
    UNDEFINED_CMD   = -2,
    CMD_ERROR       = -1,
    NO_ERROR        = 0,
} CMD_RETURN_TYPES;

extern char READY_TO_RECEIVE;

/*************************************************************
 * FUNCTION: configure_device_state()
 * -----------------------------------------------------------
 * This function receives configuration data and sets it
 * within the SealHAT device.
 *************************************************************/
CMD_RETURN_TYPES configure_sealhat_device();

/*************************************************************
 * FUNCTION: retrieve_data_state()
 * -----------------------------------------------------------
 * This function streams all data from the device's external
 * flash to the PC via USB connection. One page's worth of
 * data is sent at a time.
 *************************************************************/
CMD_RETURN_TYPES retrieve_sealhat_data();

/*************************************************************
 * FUNCTION: listen_for_commands()
 * -----------------------------------------------------------
 * This function listens for a command coming in over USB
 * connection. The listening loop may also be broken by setting
 * the global variable STOP_LISTENING to true.
 *************************************************************/
SYSTEM_COMMANDS listen_for_commands();

/**
 * Initializes the resources needed for the data aggregation task.
 * @return system error code. ERR_NONE if successful, or negative if failure.
 */
int32_t SERIAL_task_init(void);

/**
 * The data aggregation task. Only use as a task in RTOS, never call directly.
 */
void SERIAL_task(void* pvParameters);

#endif /* SEAL_SERIAL_H_ */