/*
 * seal_GPS.c
 *
 * Created: 5/11/2018 2:36:10 PM
 *  Author: Anthony Koutroulis
 */

 #include "seal_GPS.h"
 #include "seal_DATA.h"

TaskHandle_t xGPS_th;                    // GPS task handle
StaticTask_t xGPS_taskbuf;               // task buffer for the GPS task
StackType_t  xGPS_stack[GPS_STACK_SIZE]; // static stack allocation for GPS task
static xTimerHandle  xGPS_timer;         // handle for the hourly timer
static StaticTimer_t xGPS_timerbuf;      // static buffer to hold timer state


int32_t GPS_task_init(void *profile)
{
    int32_t err;    /* for catching API errors */
    
    // TODO - remove when EEPROM is configured
    eeprom_data.sensorConfigs.gpsConfig.idleRate = 75000; 
    eeprom_data.sensorConfigs.gpsConfig.activeRate = 30000;

    /* initialize the GPS module */
    gpio_set_pin_level(GPS_EXT_INT, false);
    err = gps_init_i2c(&I2C_GPS) ? ERR_NOT_INITIALIZED : ERR_NONE;

    /* force the GPS to stay awake until configuration is complete */
    gpio_set_pin_level(GPS_EXT_INT, true);

    /* verify/load GPS settings, set up NAV polling, and disable output for now */
    if (ERR_NONE == err && GPS_SUCCESS != gps_checkconfig()) {
        err =   gps_reconfig() ? ERR_NOT_READY : ERR_NONE;  // TODO change to 0
    }

    if (err) { // TODO what to do if this fails? Should be handled in SW  
        gpio_toggle_pin_level(LED_RED);
    } else {
        /* save configurations */
        gps_savecfg(0xFFFF);
        
        /* create a timer with a one hour period for controlling sensors */
        configASSERT(!configUSE_16_BIT_TICKS);
        xGPS_timer = xTimerCreateStatic(   "GPSTimer",                 /* text name of the timer       */
                                            pdMS_TO_TICKS(60000),       /* timer period in ticks (1min) */
                                            pdFALSE,                    /* manual reload after expire   */
                                            (void*)0,                   /* id of expiration counter     */
                                            GPS_movement_cb,            /* timer expiration callback    */
                                            &xGPS_timerbuf);           /* timer data buffer            */
        
        if (NULL == xGPS_timer) { /* if the timer was not created */
            err = DEVICE_ERR_TIMEOUT; // TODO determine proper handling
        }
        
        /* create the task, return ERR_NONE or ERR_NO_MEMORY if the task creation failed */
        xGPS_th = xTaskCreateStatic(GPS_task, "GPS", GPS_STACK_SIZE, (void *)profile, GPS_TASK_PRI, xGPS_stack, &xGPS_taskbuf);
        configASSERT(xGPS_th);
    }
    
    return err;
}

void GPS_task(void *pvParameters)
{
    uint16_t    activehours;        /* number of hours in a day the gps is on   */
    uint16_t    moveminutes;        /* high resolution time(m) allowed per hour */
    uint32_t    samplerate;         /* rate to collect fix and report message   */
    uint32_t    ulNotifyValue;      /* holds the notification bits from the ISR */
    int32_t     err;                /* for catching API errors                  */
    BaseType_t  xResult;            /* holds return value of blocking function  */
    TickType_t  xMaxBlockTime;      /* max time to wait for the task to resume  */
    static GPS_MSG_t gps_msg;	    /* holds the GPS message to store in flash  */
    
    (void)pvParameters;
    activehours = 0;
    eeprom_data.sensorConfigs.gpsConfig.idleRate = 75000;    // TODO remove after EEPROM is set
    eeprom_data.sensorConfigs.gpsConfig.activeRate = 30000;    // TODO remove after EEPROM is set
    
    for(int i = 0; i < 24; i++) {   /* determine the amount of active hours per day */
        activehours += (eeprom_data.sensorConfigs.gpsConfig.activeHour >> i) & 1;
    }
    activehours = 24; // TODO remove, testing only
    moveminutes = GPS_MAXMOVE / activehours;   /* determine the high-res time per hour */
    
    /* set the default sample rate */ // TODO: allow flexibility in message rate or fix to sample rate
    samplerate = eeprom_data.sensorConfigs.gpsConfig.idleRate;

    /* update the maximum blocking time to current FIFO full time + <max sensor time> */
    xMaxBlockTime = pdMS_TO_TICKS(samplerate);	    // TODO calculate based on registers

    /* initialize the message header */
    dataheader_init(&gps_msg.header);
    gps_msg.header.id  = DEVICE_ID_GPS;

    /* ensure the TX_RDY interrupt is deactivated */
    gpio_set_pin_level(GPS_TXD, true);
    
    /* clear the GPS FIFO before enabling interrupt */
    gps_readfifo();
    
    /* enable the data ready interrupt (TxReady) */
    ext_irq_register(GPS_TXD, GPS_isr_dataready);
    
    /* put the device to sleep until its period is up */
    gpio_set_pin_level(GPS_EXT_INT, false);
    gps_nap(samplerate);
    for (;;) {
        /* wait for notification from ISR, returns `pdTRUE` if task, else `pdFALSE` */
        xResult = xTaskNotifyWait( GPS_NOTIFY_NONE, /* bits to clear on entry       */
                                   GPS_NOTIFY_ALL,  /* bits to clear on exit        */
                                   &ulNotifyValue,  /* stores the notification bits */
                                   xMaxBlockTime ); /* max wait time before error   */

        
        if (pdPASS == xResult) { /* there was an interrupt before the rest period was up */
            
            /* if device should shutdown */
            if (GPS_NOTIFY_SHUTDOWN & ulNotifyValue) {
                /* wake it up */
                gpio_set_pin_level(GPS_EXT_INT, true);
                os_sleep(pdMS_TO_TICKS(5));
                
                /* tell it to go into low power mode indefinitely */
                gpio_set_pin_level(GPS_EXT_INT, false);
                gps_nap(0);
                
                vTaskSuspend(xGPS_th);
            }
            
            /* if requested, reload the high-res movement minutes */
            if (GPS_NOTIFY_HOUR & ulNotifyValue) {
                moveminutes = GPS_MAXMOVE / activehours;
            }
            
            /* if the ISR indicated that data is ready */
            if (GPS_NOTIFY_TXRDY & ulNotifyValue) {
                /* keep the GPS awake until the interrupt is handled */
                gpio_set_pin_level(GPS_EXT_INT, true);
                os_sleep(pdMS_TO_TICKS(5));

                /* copy the GPS FIFO over I2C */
                err = gps_readfifo() ? ERR_TIMEOUT : ERR_NONE;

                /* put device back to sleep */
                gpio_set_pin_level(GPS_EXT_INT, false);
                gps_nap(samplerate);
                
                /* and log it, noting communication error if needed */
                GPS_log(&gps_msg, err, DEVICE_ERR_COMMUNICATIONS);
                xMaxBlockTime = pdMS_TO_TICKS(samplerate);
            }
            
            /* if motion has been detected by the IMU */
            if (GPS_NOTIFY_MOTION & ulNotifyValue && moveminutes) {
                /* block the state machine from sending another rate change request */
                xEventGroupSetBits(xSYSEVENTS_handle, EVENT_GPS_COOLDOWN);
                
                /* keep the GPS awake until the interrupt is handled */
                gpio_set_pin_level(GPS_EXT_INT, true);
                
                /* decrement the available high-res minutes and set the rate */
                moveminutes--;
                samplerate = eeprom_data.sensorConfigs.gpsConfig.activeRate;
                err = gps_setrate(samplerate) ? ERR_NO_CHANGE : ERR_NONE;
                
                /* if failure, try again. else save configurations and start a one minute timer */
                if(err) {
                    xTaskNotify(xGPS_th, GPS_NOTIFY_MOTION, eSetBits);
                } else {
                    gps_savecfg(0xFFFF);
                    
                    /* put device back to sleep */
                    gpio_set_pin_level(GPS_EXT_INT, false);
                    gps_nap(samplerate);
                    
                    xMaxBlockTime = pdMS_TO_TICKS(samplerate);
                    xTimerChangePeriod(xGPS_timer, pdMS_TO_TICKS(60000), 0);
                }
            }
            
            /* if the high precision motion timer has expired */
            if (GPS_NOTIFY_REVERT & ulNotifyValue) {
                /* keep the GPS awake until the interrupt is handled */
                gpio_set_pin_level(GPS_EXT_INT, true);
                
                /* revert the rate back to the resting rate */
                samplerate = eeprom_data.sensorConfigs.gpsConfig.idleRate;
                err = gps_setrate(samplerate) ? ERR_NO_CHANGE : ERR_NONE;
                
                /* try again if not successful */
                if (err) {
                    xTaskNotify(xGPS_th, GPS_NOTIFY_REVERT, eSetBits);
                } else {
                    gps_savecfg(0xFFFF);
                    
                    /* put device back to sleep */
                    gpio_set_pin_level(GPS_EXT_INT, false);
                    gps_nap(samplerate);
                    
                    xMaxBlockTime = pdMS_TO_TICKS(samplerate);
                }
            }
        } else { /* there was no interrupt and the GPS has slept for one period */
            gpio_set_pin_level(GPS_EXT_INT, true);
            os_sleep(pdMS_TO_TICKS(5));
                 
            /* if it is the first wakeup */
            if (xMaxBlockTime == samplerate) {   
                /* load fix settings and wait for a fix/FIFO interrupt */
                gps_loadcfg(0xFFFF);
                gpio_set_pin_level(GPS_EXT_INT, false);
                xMaxBlockTime = pdMS_TO_TICKS(GPS_WAIT_TIME); /* allow time to receive a fix */
            } else { 
                /* there was no fix, resolve timeout error */
                err = gps_checkfifo();
                gpio_set_pin_level(GPS_EXT_INT, false);
                if (0 > err) {
                    /* if the GPS doesn't respond, try again soon */
                    xMaxBlockTime = pdMS_TO_TICKS(GPS_WAIT_TIME);
                } else if (GPS_FIFOSIZE < err) {
                    /* if the FIFO has "overflown", clear and log error */
                    err = gps_readfifo() ? ERR_TIMEOUT : ERR_NONE;
                    GPS_log(&gps_msg, err, DEVICE_ERR_OVERFLOW | DEVICE_ERR_TIMEOUT);
                    gps_nap(samplerate);
                } else {
                    /* GPS is responsive, allow full sleep cycle */
                    xMaxBlockTime = pdMS_TO_TICKS(samplerate);
                    xTaskNotify(xGPS_th, GPS_NOTIFY_TXRDY, eSetBits);
                }
            }            
        }
    } // END FOREVER LOOP
}

void GPS_isr_dataready(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;  // will be set to true by notify if we are awakening a higher priority task

    /* Notify the GPS task that the FIFO has enough data to trigger the TxReady interrupt */
    xTaskNotifyFromISR(xGPS_th, GPS_NOTIFY_TXRDY, eSetBits, &xHigherPriorityTaskWoken);

    /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
    should be performed to ensure the interrupt returns directly to the highest
    priority task.*/
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void GPS_movement_cb(TimerHandle_t xTimer)
{
    configASSERT(xTimer);
    
    /* allow the GPS to receive IMU events again */
    xEventGroupClearBits(xSYSEVENTS_handle, EVENT_GPS_COOLDOWN);
    
    /* tell the GPS to revert to resting rate, try again if busy */
    xTaskNotify(xGPS_th, GPS_NOTIFY_REVERT, eSetBits);
}

int32_t GPS_log(GPS_MSG_t *msg, const int32_t ERR, const DEVICE_ERR_CODES_t ERR_CODES)
{
    uint16_t    logcount;           /* how many log entries were parsed */
    uint16_t    logsize;            /* size in bytes of the log message */

    /* set the timestamp and any error flags to the log message */
    timestamp_FillHeader(&msg->header);
    msg->header.id = DEVICE_ID_GPS;
    
    if (ERR < 0) {
        /* if an error occurred with the FIFO, log the error */
        msg->header.id  |= ERR_CODES;
    }
    
    /* otherwise, extract the GPS data and log it */
    logcount = gps_parsefifo(msg->log, GPS_LOG_SIZE);
    msg->header.size = logcount * sizeof(gps_log_t);
    logsize = sizeof(DATA_HEADER_t) + msg->header.size;

    /* write the message to flash */
    return ctrlLog_write((uint8_t*)msg, logsize);
}