/*
 * seal_RTOS.c
 *
 * Created: 30-Apr-18 23:23:33
 *  Author: Ethan
 */
#include "seal_RTOS.h"

void vApplicationIdleHook(void)
{
   sleep(PM_SLEEPCFG_SLEEPMODE_STANDBY_Val);
}

void vApplicationTickHook(void)
{
    #ifdef SEAL_DEBUG
    gpio_toggle_pin_level(LED_RED);
    #endif
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName )
{
    STACK_OVERFLOW_PACKET_t msg;
    (void)xTask;

    // write the task name and time that "overflown" to the RTC user register to be dealt with after reset
    hri_rtcmode0_write_GP_reg(RTC, 0, (uint32_t)pcTaskName);
    hri_rtcmode0_write_GP_reg(RTC, 1, _calendar_get_counter(&RTC_CALENDAR.device));

    // request a system reset, while(1) prevents a return just in case and should trigger watchdog
    _reset_mcu();
    while(1) {;}
}

int32_t checkResetReason(void) {
    enum reset_reason cause = _get_reset_reason();      // store the reset cause

    switch(cause) {
        case RESET_REASON_SYST   : gpio_set_pin_level(LED_RED, false);  // only blink LED, data is filled in function that requested reset
                                   delay_ms(100);
                                   gpio_set_pin_level(LED_RED, true);
                                   break;
        case RESET_REASON_WDT    :
        case RESET_REASON_POR    :
        case RESET_REASON_BOD12  :
        case RESET_REASON_BOD33  :
        case RESET_REASON_EXT    :
        case RESET_REASON_BACKUP :
        default:                   gpio_set_pin_level(LED_RED, false);
                                   delay_ms(100);
                                   gpio_set_pin_level(LED_RED, true);
                                   hri_rtcmode0_set_GP_reg(RTC, 0, cause);
                                   hri_rtcmode0_write_GP_reg(RTC, 1, _calendar_get_counter(&RTC_CALENDAR.device));
    };
    return (int32_t)cause;
}

void dataheader_init(DATA_HEADER_t* header)
{
    header->size        = 0;
    header->id          = 0;
    header->timestamp   = 0;
    header->packetCount = 0;
    header->startSym    = MSG_START_SYM;
}

void timestamp_FillHeader(DATA_HEADER_t* header)
{
    // get the time in seconds since (the custom set) epoch
    header->timestamp = _calendar_get_counter(&RTC_CALENDAR.device);
    header->packetCount++;
}

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/*************************************************************
 * FUNCTION: eeprom_save_configs()
 * -----------------------------------------------------------
 * This function writes the SealHAT device's sensor and
 * configuration data out to the chip's onboard EEPROM.
 *
 * Parameters:
 *      config_settings : Pointer to struct of config settings.
 *
 * Returns:
 *      The error value of the flash_write operation.
 *************************************************************/
uint32_t eeprom_save_configs(EEPROM_STORAGE_t *config_settings)
{
    uint32_t retVal;

    /* Flash must be erased before a new value may be written to it. */
    retVal = flash_erase(&FLASH_NVM, CONFIG_BLOCK_BASE_ADDR, sizeof(EEPROM_STORAGE_t));

    /* If the erase operation succeeded, write the new data to the EEPROM.
     * Otherwise, return the error value. */
    if(retVal == ERR_NONE)
    {
        /* Cast pointer to config struct to a uint8 pointer. */
        retVal = flash_write(&FLASH_NVM, CONFIG_BLOCK_BASE_ADDR, (uint8_t *) config_settings, sizeof(EEPROM_STORAGE_t));
    }

    return (retVal);
}

/*************************************************************
 * FUNCTION: eeprom_read_configs()
 * -----------------------------------------------------------
 * This function reads the SealHAT device's sensor and
 * configuration settings from the onboard EEPROM.
 *
 * Parameters:
 *      config_settings : Pointer to struct of config settings.
 *
 * Returns:
 *      The error value of the flash_read operation.
 *************************************************************/
uint32_t eeprom_read_configs(EEPROM_STORAGE_t *config_settings)
{
    /* Cast pointer to config struct to a uint8 pointer. */
    return(flash_read(&FLASH_NVM, CONFIG_BLOCK_BASE_ADDR, (uint8_t *) config_settings, sizeof(EEPROM_STORAGE_t)));
}