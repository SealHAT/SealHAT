/*
 * seal_ECG.c
 *
 * Created: 6/7/2018 4:21:32 AM
 *  Author: Anthony Koutroulis
 */

 #include "seal_ECG.h"
 #include "seal_DATA.h"

 #define ECG_DATA_READY (0x01)

 TaskHandle_t           xECG_th;                    // ECG task handle
 static StaticTask_t    xECG_taskbuf;               // task buffer for the ECG task
 static StackType_t     xECG_stack[ECG_STACK_SIZE]; // static stack allocation for ECG task

 #ifdef SEAL_DEBUG
 static UBaseType_t uxECG_highwatermark;
 #endif

void ECG_isr_dataready(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;  // will be set to true by notify if we are awakening a higher priority task

    /* Notify the GPS task that the FIFO has enough data to trigger the TxReady interrupt */
    vTaskNotifyGiveFromISR(xECG_th, &xHigherPriorityTaskWoken);

    /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
    should be performed to ensure the interrupt returns directly to the highest
    priority task.*/
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

 int32_t ECG_task_init(void)
 {
    if (ERR_NONE != ecg_spi_init()) {
        return ERR_NOT_INITIALIZED;
    }

    if (ERR_NONE != ecg_init()) {
        return ERR_NOT_INITIALIZED;
    }
    ecg_synch();
    
    xECG_th = xTaskCreateStatic(ECG_task, "ECG", ECG_STACK_SIZE, NULL, ECG_TASK_PRI, xECG_stack, &xECG_taskbuf);
    configASSERT(xECG_th);
    return ERR_NONE;
 }

 void ECG_task(void *pvParameters)
 {
    int32_t     count = 0;              /* track the number of samples read from FIFO */
    uint32_t    ulNotifyValue;          // notification value from ISRs
    BaseType_t  xResult;                // holds return value of blocking function
    static ECG_MSG_t ecg_msg;	        /* holds the ECG message to store in flash  */
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 1024 );


    (void)pvParameters;

    #ifdef SEAL_DEBUG  /* test for stack usage */
    uxECG_highwatermark = uxTaskGetStackHighWaterMark(xECG_th);
    #endif

    /* register the data ready interrupt */
    ext_irq_register(MOD_INT1, ECG_isr_dataready);

    /* prepare the logging header */
    dataheader_init(&ecg_msg.header);
    ecg_msg.header.id   = DEVICE_ID_EKG;
    ecg_msg.header.size = sizeof(ECG_SAMPLE_t)*ECG_LOG_SIZE;

    /* clear the fifo */
    ecg_fifo_reset();

    /* main task loop */
    for (;;){
         xResult = xTaskNotifyWait( pdFALSE,          /* Don't clear bits on entry. */
                                    ULONG_MAX,        /* Clear all bits on exit. */
                                    &ulNotifyValue,   /* Stores the notified value. */
                                    xMaxBlockTime );  /* Max time to block before writing an error packet */

        if (pdPASS == xResult) {
            /* if the watermark was hit */
            if (ECG_DATA_READY) {
                portENTER_CRITICAL();
                count = ecg_get_sample_burst(ecg_msg.log, ECG_LOG_SIZE);
                portEXIT_CRITICAL();

                timestamp_FillHeader(&ecg_msg.header);
                if (0 < count) {
                    ecg_msg.header.size = sizeof(ECG_SAMPLE_t)*count;
                    ctrlLog_write((uint8_t*)&ecg_msg, sizeof(ECG_MSG_t));
                } else {
                    ecg_msg.header.id   |= DEVICE_ERR_COMMUNICATIONS;
                    ecg_msg.header.size = 0;
                    ctrlLog_write((uint8_t*)&ecg_msg, sizeof(DATA_HEADER_t));
                    ecg_msg.header.id &= ~(DEVICE_ERR_MASK);
                }
                #ifdef SEAL_DEBUG  /* test for stack usage */
                uxECG_highwatermark = uxTaskGetStackHighWaterMark(xECG_th);
                os_sleep(1);
                #endif
            }
            else {
                ecg_msg.header.id |= DEVICE_ERR_TIMEOUT;
                ctrlLog_write((uint8_t*)&ecg_msg, sizeof(ECG_MSG_t));
            }
        } // END Notification response
        else {
            ecg_synch();
        }
    } // END forever loop
 }



