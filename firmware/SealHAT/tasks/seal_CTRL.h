/*
 * seal_MSG.h
 *
 * Created: 30-Apr-18 22:47:57
 *  Author: Ethan
 */

#ifndef SEAL_MSG_H_
#define SEAL_MSG_H_

#include "seal_RTOS.h"
#include "seal_Types.h"

#define CTRL_STACK_SIZE                 (1000 / sizeof(portSTACK_TYPE))
#define CTRL_TASK_PRI                   (configMAX_PRIORITIES - 1)

extern TaskHandle_t       xCTRL_th;           // Message accumulator for USB/MEM

/**
 * This function is the ISR callback intended for use with the VBUS interrupt.
 * It will be called on the rising and falling edge of VBUS.
 */
void vbus_detection_cb(void);

/**
 * Initializes the resources needed for the control task.
 * @return system error code. ERR_NONE if successful, or negative if failure (ERR_NO_MEMORY likely).
 */
int32_t CTRL_task_init(void);

/**
 * The control task. Only use as a task in RTOS, never call directly.
 */
void CTRL_task(void* pvParameters);

/**
 *  Updates the hourly timer used in sensor scheduling
 */
void CTRL_timer_update(TimerHandle_t xTimer);

/**
 *  Indicate to the CTRL the start of a new hour
 */
void vHourlyTimerCallback( TimerHandle_t xTimer );

/**
 *  Perform hourly bookkeeping, update time from GPS, check and set active sensors
 */
void CTRL_hourly_update(void);

#endif /* SEAL_MSG_H_ */
