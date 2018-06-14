/*
 * seal_ENV.c
 *
 * Created: 30-Apr-18 22:53:14
 *  Author: Ethan
 */
#include "seal_ENV.h"
#include "seal_DATA.h"
#include "max44009/max44009.h"
#include "si705x/si705x.h"

#define TEMP_READ_TIME      (pdMS_TO_TICKS(8)) // time to block between reading start and get in ms

TaskHandle_t xENV_th;                       // environmental sensors task (light and temp)
StaticTask_t xENV_taskbuf;                  // task buffer for the ENV task
StackType_t  xENV_stack[ENV_STACK_SIZE];    // static stack allocation for ENV task

int32_t ENV_task_init(const int32_t period)
{
    xENV_th = xTaskCreateStatic(ENV_task, "ENV", ENV_STACK_SIZE, (void*)period, ENV_TASK_PRI, xENV_stack, &xENV_taskbuf);
    configASSERT(xENV_th);

    return ERR_NONE;
}

void ENV_task(void* pvParameters)
{
    static ENV_MSG_t lightMsg;             // data buffer for storing light samples to send
    static ENV_MSG_t tempMsg;             // data buffer for storing temperature samples to send
    int32_t     err;                    // for catching API errors
    TickType_t  xPeriod;                // the period of the sampling in seconds
    TickType_t  xLastWakeTime;          // last wake time variable for timing
    (void)pvParameters;

    // initialize the temperature sensor
    err = si705x_init(&I2C_ENV);

    // initialize the light sensor
    err = max44009_init(&I2C_ENV, LIGHT_ADD_GND);

    // set the header data
    dataheader_init(&lightMsg.header);
    dataheader_init(&tempMsg.header);
    lightMsg.header.size     = ENV_LOG_SIZE * sizeof(uint16_t);
    tempMsg.header.size      = ENV_LOG_SIZE * sizeof(uint16_t);
    lightMsg.header.id       = DEVICE_ID_LIGHT;
    tempMsg.header.id        = DEVICE_ID_TEMPERATURE;

    // Initialize the xLastWakeTime variable with the current time.
    xPeriod       = pdMS_TO_TICKS((uint32_t)pvParameters * 1000);
    xLastWakeTime = xTaskGetTickCount();

    for(;;) {

        uint_fast8_t i;
        for(i = 0; i < ENV_LOG_SIZE; i++) {
            // initialize the start time, or re-init if the task was suspended
            if((xLastWakeTime + xPeriod) < xTaskGetTickCount()) {
                xLastWakeTime = xTaskGetTickCount();
            }
            vTaskDelayUntil(&xLastWakeTime, xPeriod);

            // start an asynchronous temperature reading
            portENTER_CRITICAL();
            err = si705x_measure_asyncStart();
            portEXIT_CRITICAL();

            //  read the light level
            portENTER_CRITICAL();
            err = max44009_read(&lightMsg.data[i]);
            portEXIT_CRITICAL();

            // wait for the temperature sensor to finish
            os_sleep(TEMP_READ_TIME);

            // get temp
            portENTER_CRITICAL();
            err = si705x_measure_asyncGet(&tempMsg.data[i], 250, true);
            portEXIT_CRITICAL();
            if(ERR_BAD_DATA == err) {
                tempMsg.data[i] &= 0x8000;
            }

        } // for loop filling the packet

        // reset the message header and set the timestamp
        timestamp_FillHeader(&lightMsg.header);
        timestamp_FillHeader(&tempMsg.header);
        // send data to the CTRL task once done
        err = ctrlLog_write((uint8_t*)&tempMsg, sizeof(ENV_MSG_t));
        err = ctrlLog_write((uint8_t*)&lightMsg, sizeof(ENV_MSG_t));
    }
} 