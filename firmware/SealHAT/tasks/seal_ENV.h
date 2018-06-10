/*
 * seal_ENV.h
 *
 * Created: 30-Apr-18 22:40:35
 *  Author: Ethan
 */
#include "seal_RTOS.h"
#include "seal_Types.h"

#ifndef SEAL_ENV_H_
#define SEAL_ENV_H_

#define ENV_STACK_SIZE                  (500 / sizeof(portSTACK_TYPE))  // high water mark of 47 on 26MAY18
#define ENV_TASK_PRI                    (tskIDLE_PRIORITY + 2)

extern TaskHandle_t xENV_th;        // environmental sensors task (light and temp)

/**
 * Initializes the resources needed for the environmental sensor task.
 *
 * @return system error code. ERR_NONE if successful, or negative if failure (ERR_NO_MEMORY likely).
 */
int32_t ENV_task_init(const int32_t period);

/**
 * The environmental task. only use as a task in RTOS.
 */
void ENV_task(void* pvParameters);

#endif /* SEAL_ENV_H_ */