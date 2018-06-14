/*
 * seal_SERIAL.c
 *
 * Created: 27-May-18 2:35:51 PM
 *  Author: Krystine
 */

#include "seal_SERIAL.h"
#include "seal_DATA.h"
#include "sealPrint.h"
#include "storage\flash_io.h"

TaskHandle_t        xSERIAL_th;                                 // Message accumulator for USB/MEM
static StaticTask_t xSERIAL_taskbuf;                            // task buffer for the SERIAL task
static StackType_t  xSERIAL_stack[SERIAL_STACK_SIZE];           // static stack allocation for SERIAL task

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
    static uint8_t menu[] = USB_TEXT_ADVENTURE_MENU; // "\n(c)onfigure, (v)erify config, (d)ownload data, (s)tream data, (f)orce Logging, (r)eset\n"

    /* Receive and commands forever. */
    for(;;)
    {
        // if the USB is configured, otherwise
        if(usb_state() == USB_Configured) {
            // DTR signal signifies a host is active and listening
            if(usb_dtr()) {
                // if we are currently streaming to USB then do not print a menu
                if(!(xEventGroupGetBits(xSYSEVENTS_handle) & EVENT_LOGTOUSB)) {
                    /* Print menu to console. */
                    do {
                        err = usb_write(menu, (sizeof(menu) - 1));
                    } while((err != USB_OK) || !usb_dtr());

                    /* Wait for command to be given. */
                    cmd = listen_for_commands();

                    switch(cmd) {
                        case CONFIGURE_DEV:
                        {
                            /* Notify CTRL that the device is to undergo configuration and wait for green light */

                            // TODO output "please wait while devices shutdown" and then "configuring device" messages
                            xEventGroupSetBits(xSYSEVENTS_handle, EVENT_CONFIG_START);
                            ulTaskNotifyTake( pdTRUE, portMAX_DELAY); // TODO add timeout and error handling

                            if (ERR_NONE != configure_sealhat_device()) {
                                usb_put(OPERATION_ERROR);
                            }
                            else {
                                usb_put(OPERATION_SUCCESS);
                            }
                            xEventGroupSetBits(xSYSEVENTS_handle, EVENT_CONFIG_STOP);
                            break;
                        }
                        case VERIFY_CONFIG:
                        {
                            SYSTEM_CONFIG_t packet;
                            dataheader_init(&packet.header);
                            packet.header.id = DEVICE_ID_CONFIG;
                            packet.header.size = sizeof(SENSOR_CONFIGS_t) + sizeof(uint32_t);
                            packet.sensorConfigs = eeprom_data.sensorConfigs;

                            // fill CRC32 of the packet
                            packet.crc32 = 0xFFFFFFFF;
                            crc_sync_crc32(&CRC_0, (uint32_t*)&packet.sensorConfigs, sizeof(SENSOR_CONFIGS_t)/sizeof(uint32_t), &packet.crc32);
                            packet.crc32 ^= 0xFFFFFFFF;

                            timestamp_FillHeader(&packet.header);

                            do {
                                err = usb_write((uint8_t*)&packet, sizeof(SYSTEM_CONFIG_t));
                            } while((err != USB_OK) || !usb_dtr());
                            break;
                        }
                        case DOWNLOAD_DATA:
                        {
                            /* Notify CTRL that the device is to undergo data retrieval and wait for sensors to sleep */
                            xEventGroupSetBits(xSYSEVENTS_handle, EVENT_RETRIEVE);
                            ulTaskNotifyTake( pdTRUE, portMAX_DELAY); // TODO add timeout and error handling

                            if (ERR_NONE != retrieve_sealhat_data()) {
                                usb_put(OPERATION_ERROR);
                            }
                            else {
                                usb_put(OPERATION_SUCCESS);
                            }
                            xEventGroupClearBits(xSYSEVENTS_handle, EVENT_RETRIEVE);
                            break;
                        }
                        case STREAM_DATA:
                        {
                            xEventGroupSetBits(xSYSEVENTS_handle, EVENT_LOGTOUSB);
                            break;
                        }
                        case FORCE_FLASH:
                        {
                            xEventGroupSetBits(xSYSEVENTS_handle, EVENT_LOGTOFLASH);
                            usb_put(OPERATION_SUCCESS);
                            // TODO: wipe start date? or leave intact? does it matter?
                            break;
                        }
                        case RESET_SYSTEM:
                        {
                            _reset_mcu();
                            while(1) {;}
                            break;
                        }
                        default:
                        {
                            err = UNDEFINED_CMD;
                            usb_put(OPERATION_ERROR);
                            break;
                        }
                    }
                } // Logging Active
                else {
                    // listen for a stop streaming command, 's'
                    int32_t stopCmd = usb_get();
                    if(stopCmd == 's') {
                        xEventGroupClearBits(xSYSEVENTS_handle, EVENT_LOGTOUSB);
                        usb_write("\0\0\0\n\n", 5);
                    }
                }
            }
            else {
                // USB connected but no terminal ready signal
                xEventGroupClearBits(xSYSEVENTS_handle, EVENT_LOGTOUSB);
            }

        } /* END if(usb_state() == USB_Configured) */
        else {
            /**
             * If usb is disconnected, make sure USB streaming is disabled.
             * Leave flash bit as is, since it will either be activated by the calendar or by a command
             */
            xEventGroupClearBits(xSYSEVENTS_handle, EVENT_LOGTOUSB);
            eBits = xEventGroupGetBits(xSYSEVENTS_handle);

            /* Double check VBUS, but if USB is not enumerated then it probably isn't plugged in */
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
    SYSTEM_CONFIG_t  tempConfigStruct;  /* Hold configuration settings read over USB. */
    CMD_RETURN_TYPES errVal;            /* Return value for the function call. */
    bool             packetOK;          /* Checking for incoming packet integrity. */
    uint32_t         retVal;            /* Return value of USB function calls. */
    uint32_t         crc32_check;

    /* Initialize return value. */
    errVal = CMD_ERROR;

    /* Reinitialize loop control variable. */
    STOP_LISTENING = false;
    packetOK       = false;

    /* Send the ready to receive signal. */
    do {
        retVal = usb_put(READY_TO_RECEIVE);
    } while((retVal != USB_OK) || (!usb_dtr()));

    /* Wait for configuration packet to arrive. */
    do {
        retVal = usb_read(&tempConfigStruct, sizeof(SYSTEM_CONFIG_t));
    } while((retVal == 0) || (STOP_LISTENING == true));

    // check the packet with CRC32
    crc32_check = 0xFFFFFFFF;
    crc_sync_crc32(&CRC_0, (uint32_t*)&tempConfigStruct.sensorConfigs, sizeof(SENSOR_CONFIGS_t)/sizeof(uint32_t), &crc32_check);
    crc32_check ^= 0xFFFFFFFF;
    packetOK = ((tempConfigStruct.header.id == DEVICE_ID_CONFIG) && (crc32_check == tempConfigStruct.crc32));

    if(packetOK) {
        /* Set the system time*/
        hri_rtcmode0_write_COUNT_reg(RTC, tempConfigStruct.header.timestamp);

        /* Temp struct has passed the test and may become the real struct. */
        eeprom_data.sensorConfigs = tempConfigStruct.sensorConfigs;

        /* Save new configuration settings. */
        errVal = eeprom_save_configs(&eeprom_data);
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

    /* Keep listening for a command until one is received or until the kill command (STOP_LISTENING) is received. */
    while(noCommand && usb_dtr() && !STOP_LISTENING) {
        command = usb_get();

        /* If a command was given, break loop. */
        if((command == CONFIGURE_DEV) || (command == VERIFY_CONFIG) || (command == DOWNLOAD_DATA)
            || (command == STREAM_DATA) || (command == FORCE_FLASH) || (command == RESET_SYSTEM))
        {
            noCommand = false;
        }
    }

    /* If the loop was broken before a valid command was received, set the return value
     * to NO COMMAND. */
    if(noCommand || !usb_dtr()) {
        command = NO_COMMAND;
    }

    return ((SYSTEM_COMMANDS) command);
}