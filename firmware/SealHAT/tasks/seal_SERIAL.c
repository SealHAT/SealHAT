/*
 * seal_SERIAL.c
 *
 * Created: 27-May-18 2:35:51 PM
 *  Author: Krystine
 */ 

#include "seal_SERIAL.h"

TaskHandle_t        xSERIAL_th;                                 // Message accumulator for USB/MEM
static StaticTask_t xSERIAL_taskbuf;                            // task buffer for the SERIAL task
static StackType_t  xSERIAL_stack[SERIAL_STACK_SIZE];           // static stack allocation for SERIAL task

char READY_TO_RECEIVE = 'r';    /* Character sent over USB to device to initiate packet transfer */
bool STOP_LISTENING;            /* This should be set to true if the device should no longer listen for incoming commands. */

int32_t SERIAL_task_init(void)
{
    xSERIAL_th = xTaskCreateStatic(SERIAL_task, "COMM", SERIAL_STACK_SIZE, NULL, SERIAL_TASK_PRI, xSERIAL_stack, &xSERIAL_taskbuf);
    configASSERT(xSERIAL_th);
    vTaskSuspend(xSERIAL_th);   /* suspend until requested by seal_CTRL on VBUS detect */

    return ERR_NONE;
}

void SERIAL_task(void* pvParameters)
{
    int32_t err;
    (void)pvParameters;
    SYSTEM_COMMANDS cmd;
    EventBits_t eBits;
    char option;
    bool done;
    static uint8_t menu[] = "(c)onfigure, (r)etrieve data, (s)tream data\n";
    static uint8_t steamLogMenu[] = "stream (o)nly, (l)og to flash only, (b)oth\n";
    static uint8_t loggingToFlashMSG[] = "Logging data to flash.\n";

    /* Receive and commands forever. */
    for(;;)
    {
        if(usb_state() == USB_Configured && usb_dtr())
        {
            /* Print menu to console. */
            do {
                err = usb_write(menu, (sizeof(menu) - 1));
            } while((err != USB_OK) || !usb_dtr());
            
            /* Wait for command to be given. */
            cmd = listen_for_commands();
            
            /****************************************************
             * Perform specific tasks based on given command.
             *      -> configure device
             *      -> retrieve all stored data from flash
             *      -> set flags for streaming/logging data
             ****************************************************/
            switch(cmd)
            {
                case CONFIGURE_DEV: 
                {
                    /* Notify CTRL that the device is to undergo configuration and wait for green light */
                    
                    // TODO output "please wait while devices shutdown" and then "configuring device" messages
                    xEventGroupSetBits(xSYSEVENTS_handle, EVENT_CONFIG_START);
                    ulTaskNotifyTake( pdTRUE, portMAX_DELAY); // TODO add timeout and error handling
                    
                    if (ERR_NONE != configure_sealhat_device()) {
                        /* TODO: handle error writing new configs should probably fallback or notify usb */
                        gpio_toggle_pin_level(LED_RED);
                    }
                    xEventGroupClearBits(xSYSEVENTS_handle, EVENT_CONFIG_START);
                    break;
                }                    
                case RETRIEVE_DATA: 
                {
                    /* Notify CTRL that the device is to undergo data retrieval and wait for sensors to sleep */
                    xEventGroupSetBits(xSYSEVENTS_handle, EVENT_RETRIEVE);
                    ulTaskNotifyTake( pdTRUE, portMAX_DELAY); // TODO add timeout and error handling
                    
                    if (ERR_NONE != retrieve_sealhat_data()) {
                        /* TODO: handle error writing new configs should probably fallback or notify usb */
                        gpio_toggle_pin_level(LED_RED);
                    }
                    xEventGroupClearBits(xSYSEVENTS_handle, EVENT_RETRIEVE);
                    break;
                }                    
                case STREAM_DATA:
                {
                    /* Print stream or log menu to console. */
                    done = false;
                    
                    do {
                        err = usb_write(steamLogMenu, (sizeof(steamLogMenu) - 1));
                    } while((err != USB_OK) || !usb_dtr());
                    
                    /* TODO: make the while() WAY less gross.. */
                    do {
                        option = usb_get();
                        
                        if((option == 'o') || (option == 'l') || (option == 'b')) {
                            done = true;
                        }
                    } while(((option != USB_OK) || !usb_dtr()) && !done);
                    
                    /* Set the stream data flag, the log to flash flag, or both flags. */
                    if(option == 'o') {
                        xEventGroupSetBits(xSYSEVENTS_handle, EVENT_LOGTOUSB);
                        xEventGroupClearBits(xSYSEVENTS_handle, EVENT_LOGTOFLASH);
                    } else if(option == 'l') {
                        /* Print stream or log menu to console. */
                        do {
                            err = usb_write(loggingToFlashMSG, (sizeof(loggingToFlashMSG) - 1));
                        } while((err != USB_OK) || !usb_dtr());
                        
                        xEventGroupClearBits(xSYSEVENTS_handle, EVENT_LOGTOUSB);
                        xEventGroupSetBits(xSYSEVENTS_handle, EVENT_LOGTOFLASH);
                    } else {
                        xEventGroupSetBits(xSYSEVENTS_handle, (EVENT_LOGTOUSB | EVENT_LOGTOFLASH));
                    }
                    break;
                }                    
                default: err = UNDEFINED_CMD; break;
            }
            
        } /* END if(usb_state() == USB_Configured) */
        else 
        {
            /* Log data to flash only (and not USB) if USB not connected. */
            eBits = xEventGroupGetBits(xSYSEVENTS_handle);
            
            if((eBits & EVENT_LOGTOUSB) == EVENT_LOGTOUSB || (eBits & EVENT_LOGTOFLASH) != EVENT_LOGTOFLASH) {
                xEventGroupClearBits(xSYSEVENTS_handle, EVENT_LOGTOUSB);
                xEventGroupSetBits(xSYSEVENTS_handle, EVENT_LOGTOFLASH);
            }
            
            /* if the USB disconnects, suspend the serial interface task */
            if ((eBits & EVENT_VBUS) != EVENT_VBUS) {
                vTaskSuspend(xSERIAL_th);
            }
            
        } /* END else */
        os_sleep(pdMS_TO_TICKS(100));
    } /* END forever loop */
}

/*************************************************************
 * FUNCTION: configure_device_state()
 * -----------------------------------------------------------
 * This function receives configuration data and sets it 
 * within the SealHAT device. A ready signal is first sent to
 * the receiver, then the data packet is awaited. The packet
 * integrity is checked before updating configuration data.
 *
 * Parameters: none
 *
 * Returns:
 *      Status of the operation (pass, fail, error)
 *************************************************************/
CMD_RETURN_TYPES configure_sealhat_device()
{
    SENSOR_CONFIGS   tempConfigStruct;  /* Hold configuration settings read over USB. */
    CMD_RETURN_TYPES errVal;            /* Return value for the function call. */
    bool             packetOK;          /* Checking for incoming packet integrity. */
    uint32_t         retVal;            /* Return value of USB function calls. */
    
    /* Initialize return value. */
    errVal = CMD_ERROR;
    
    /* Reinitialize loop control variable. */
    STOP_LISTENING = false;
    packetOK = false;   

    /* Send the ready to receive signal. */
    do { 
        retVal = usb_put(READY_TO_RECEIVE);
    } while((retVal != USB_OK) || (!usb_dtr()));
    
    /* Wait for configuration packet to arrive. */
    do {
        retVal = usb_read(&tempConfigStruct, sizeof(SENSOR_CONFIGS));
    } while((retVal == 0) || (STOP_LISTENING == true));
    
    // TODO: error check packet
    packetOK = true; //for testing. will actually need to be checked.
    
    if(packetOK)
    {
        /* Temp struct has passed the test and may become the real struct. */
        eeprom_data.config_settings = tempConfigStruct;
        
        /* Save new configuration settings. */
        errVal = eeprom_save_configs(&eeprom_data);
        
        // TODO: ANTHONY uncomment next line and do your thing
        //xEventGroupSetBits(xSYSEVENTS_handle, EVENT_CONFIG_COMPLETE);
    }
    
    return (errVal);
}

/*************************************************************
 * FUNCTION: retrieve_data_state()
 * -----------------------------------------------------------
 * This function streams all data from the device's external
 * flash to the PC via USB connection. One page's worth of 
 * data is sent at a time.
 *
 * Parameters: none
 *
 * Returns:
 *      Success or failure code.
 *************************************************************/
CMD_RETURN_TYPES retrieve_sealhat_data()
{
    uint32_t pageIndex;         /* Loop control for iterating over flash pages. */
    uint32_t numPagesWritten;   /* Total number of pages currently written to flash. */
    uint32_t retVal;            /* USB return value for error checking/handling. */
    
    /* Initializations */
    numPagesWritten = num_pages_written();
    pageIndex = 0;
    
    /* Loop through every page that has data and send it over USB in PAGE_SIZE buffers.
     * TODO: send address or page index here too for crash recovery. */
    while(pageIndex < numPagesWritten)
    {
        /* Read a page of data from external flash. */
        retVal = flash_io_read(&seal_flash_descriptor, seal_flash_descriptor.buf_0, PAGE_SIZE_LESS);
        
        /* Write data to USB. */
        do {
           retVal = usb_write(seal_flash_descriptor.buf_0, PAGE_SIZE_LESS);
        } while((retVal != USB_OK) || (!usb_dtr()));    
        
        pageIndex++;        
    }
    
    return (NO_ERROR);
}

/*************************************************************
 * FUNCTION: listen_for_commands()
 * -----------------------------------------------------------
 * This function listens for a command coming in over USB
 * connection. The listening loop may also be broken by setting 
 * the global variable STOP_LISTENING to true. The received 
 * command is returned to the calling function.
 *
 * Parameters: none
 *
 * Returns:
 *      The received command or zero if the loop was broken
 *      before a command was received.
 *************************************************************/
SYSTEM_COMMANDS listen_for_commands()
{
    bool noCommand = true;
    char command;
    
    STOP_LISTENING = false;
    
    /* Keep listening for a command until one is received or until the kill command (STOP_LISTENING) is received. */
    while((noCommand == true) && (STOP_LISTENING == false))
    {
        command = usb_get();
        
        /* If a command was given, break loop. */
        if((command == CONFIGURE_DEV) || (command == RETRIEVE_DATA) || (command == STREAM_DATA))
        {
            noCommand = false;
        }
    }
    
    /* If the loop was broken before a valid command was received, set the return value
     * to NO COMMAND. */
    if(STOP_LISTENING == true)
    {
        command = NO_COMMAND;
    }
    
    return ((SYSTEM_COMMANDS) command);
}