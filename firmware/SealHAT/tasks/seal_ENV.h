/*
 * seal_ENV.h
 *
 * Created: 30-Apr-18 22:40:35
 *  Author: Ethan
 */
#include "seal_RTOS.h"
#include "seal_Types.h"
#include "max44009/max44009.h"
#include "si705x/si705x.h"
#include "seal_DATA.h"

#ifndef SEAL_ENV_H_
#define SEAL_ENV_H_

#define ENV_STACK_SIZE                  (500 / sizeof(portSTACK_TYPE))  // high water mark of 47 on 26MAY18
#define ENV_TASK_PRI                    (tskIDLE_PRIORITY + 2)

#define ENV_PACKET_LEGTH                (12)

typedef struct __attribute__((__packed__)){
    uint16_t light;
    uint16_t temp;
} ENV_DATA_t;

typedef struct __attribute__((__packed__)){
    DATA_HEADER_t header;
    ENV_DATA_t    data[ENV_PACKET_LEGTH];
} ENV_MSG_t;

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