/*
 * seal_DATA.h
 *
 * Created: 26-May-18 12:23:49
 *  Author: Ethan
 */


#ifndef SEAL_DATA_H_
#define SEAL_DATA_H_

#include "seal_RTOS.h"
#include "seal_Types.h"
#include "storage/flash_io.h"

#define DATA_STACK_SIZE                 (1000 / sizeof(portSTACK_TYPE))
#define DATA_TASK_PRI                   (tskIDLE_PRIORITY + 1)
#define DATA_QUEUE_LENGTH               (3000)

extern TaskHandle_t xDATA_th;                       // Message accumulator for USB/MEM
extern FLASH_DESCRIPTOR seal_flash_descriptor;      /* Declare flash descriptor. */
//extern DATA_TRANSMISSION_t usbPacket;

/**
 * Function to write to the control
 * !!! NEVER USE FROM AN ISR OR OUTSIDE OF THE RTOS CONTEXT !!!
 * This function will always write the total number of bytes requested or fail
 * the data is COPIED into the buffer. if is safe to use the data again as soon as this
 * function returns success.
 *
 * @param buff [IN] pointer to an object to write to the stream queue
 * @param LEN [IN] the size of the object in bytes
 * @return Positive Value  - the number of bytes written, which will always be the number requested.
 *         ERR_NO_RESOURCE - If there is not enough space the function will return ERR_NO_RESOURCE
 *         ERR_FAILURE     - If the mutex is taken by another task
 */
int32_t ctrlLog_write(uint8_t* buff, const uint32_t LEN);

/**
 * Function to write to the control queue from an ISR
 * This function will always write the total number of bytes requested or fail
 * the data is COPIED into the buffer. if is safe to use the data again as soon as this
 * function returns success.
 *
 * @param buff [IN] pointer to an object to write to the stream queue
 * @param LEN [IN] the size of the object in bytes
 * @return Positive Value  - the number of bytes written, which will always be the number requested.
 *         ERR_NO_RESOURCE - If there is not enough space the function will return ERR_NO_RESOURCE
 *         ERR_FAILURE     - If the mutex is taken by another task
 */
int32_t ctrlLog_writeISR(uint8_t* buff, const uint32_t LEN);

/**
 * Initializes the resources needed for the data aggregation task.
 * @return system error code. ERR_NONE if successful, or negative if failure.
 */
int32_t DATA_task_init(void);

/**
 * The data aggregation task. Only use as a task in RTOS, never call directly.
 */
void DATA_task(void* pvParameters);

#endif /* SEAL_DATA_H_ */