/*
 * seal_DATA.c
 *
 * Created: 26-May-18 12:23:37
 *  Author: Ethan
 */
#include "seal_DATA.h"
#include "seal_USB.h"
#include "sealPrint.h"
#include "storage\flash_io.h"
#include "driver_init.h"

EEPROM_STORAGE_t eeprom_data;                                       //struct containing sensor and SealHAT configurations

TaskHandle_t        xDATA_th;                                       // Message accumulator for USB/MEM
static StaticTask_t xDATA_taskbuf;                                  // task buffer for the CTRL task
static StackType_t  xDATA_stack[DATA_STACK_SIZE];                   // static stack allocation for CTRL task

//static StreamBufferHandle_t xDATA_sb;                               // stream buffer for getting data into FLASH or USB
static MessageBufferHandle_t xDATA_sb;
static uint8_t               dataQueueStorage[DATA_QUEUE_LENGTH];    // static memory for the data queue
static StaticMessageBuffer_t xDataQueueStruct;
//static StaticStreamBuffer_t xDataQueueStruct;                       // static memory for data queue data structure

FLASH_DESCRIPTOR seal_flash_descriptor;                             /* Declare flash descriptor. */

int32_t ctrlLog_write(uint8_t* buff, const uint32_t LEN)
{
    portENTER_CRITICAL();
    uint32_t err = xMessageBufferSend(xDATA_sb, (void*)buff, LEN, 0);
    portEXIT_CRITICAL();
    return err;
}

int32_t DATA_task_init(void)
{
    // enable CRC generator. This function does nothing apparently,
    // but we call it to remain consistent with API. it ALWAYS returns ERR_NONE.
    crc_sync_enable(&CRC_0);

    xDATA_sb = xMessageBufferCreateStatic(sizeof(dataQueueStorage), dataQueueStorage, &xDataQueueStruct);
    configASSERT(xDATA_sb);

    /* Read stored device settings from EEPROM and make them accessible to all devices. */
    if (eeprom_read_configs(&eeprom_data)) {
        return ERR_BAD_ADDRESS;
    }

/**************************************************************************************** NO-FLASH
    Initialize flash device(s).
//    flash_io_init(&seal_flash_descriptor, PAGE_SIZE_LESS);
****************************************************************************************/

    xDATA_th = xTaskCreateStatic(DATA_task, "DATA", DATA_STACK_SIZE, NULL, DATA_TASK_PRI, xDATA_stack, &xDATA_taskbuf);
    configASSERT(xDATA_th);

    return ERR_NONE;
}

#define RX_BUFFER_SIZE      (512)
void DATA_task(void* pvParameters)
{
    int32_t err;
    (void)pvParameters;
    static DATA_TRANSMISSION_t usbPacket;
    static uint8_t ucRXData[RX_BUFFER_SIZE];
    int32_t packetSize;
    static uint32_t idx;
    uint32_t pageIndex;         /* Loop control for iterating over flash pages. */
    uint32_t numPagesWritten;   /* Total number of pages currently written to flash. */

    /* Receive and write data forever. */
    for(;;)
    {
        // will only read out COMPLETE packets, returns the size of the packet read
        packetSize = xMessageBufferReceive(xDATA_sb, (void*)ucRXData, RX_BUFFER_SIZE, portMAX_DELAY);

/**************************************************************************************** NO-FLASH
        // Receive a page worth of data.
        idx = 0;
        while(idx < PAGE_SIZE_LESS) {
            err = xStreamBufferReceive(xDATA_sb, (usbPacket.data + idx), (PAGE_SIZE_LESS - idx), portMAX_DELAY);
            idx += err;
        }

        // Log data to flash if the appropriate flag is set.
        if((xEventGroupGetBits(xSYSEVENTS_handle) & EVENT_LOGTOFLASH) != 0)
        {
            // Write data to external flash device.
            flash_io_write(&seal_flash_descriptor, usbPacket.data, PAGE_SIZE_LESS);
        }
****************************************************************************************/

        if((xEventGroupGetBits(xSYSEVENTS_handle) & EVENT_RETRIEVE) != 0)
        {
            //TODO: ANTHONY disable all sensors/tasks here except data and serial.

            /* Initializations */
            numPagesWritten = num_pages_written();
            pageIndex = 0;

            /* Loop through every page that has data and send it over USB in PAGE_SIZE buffers.
             * TODO: send address or page index here for crash recovery. (or write out to EEPROM) */
            while(pageIndex < numPagesWritten)
            {
                /* Read a page of data from external flash. */
                flash_io_read(&seal_flash_descriptor, usbPacket.data, PAGE_SIZE_LESS);

                // setup the packet header and CRC start value, then perform CRC32
                usbPacket.startSymbol = USB_PACKET_START_SYM;
                usbPacket.crc = 0xFFFFFFFF;
                crc_sync_crc32(&CRC_0, (uint32_t*)usbPacket.data, PAGE_SIZE_LESS/sizeof(uint32_t), &usbPacket.crc);

                // complement CRC to match standard CRC32 implementations
                usbPacket.crc ^= 0xFFFFFFFF;

                /* Write data to USB. */
                if(usb_state() == USB_Configured)
                {
                    if(usb_dtr())
                    {
                        do //write USB packet. retry if no error.
                        {
                            err = usb_write(&usbPacket, sizeof(DATA_TRANSMISSION_t));
                        } while (err != ERR_NONE && err != ERR_BUSY);
                    }
                    else
                    {
                        usb_flushTx();
                    }
                }

                pageIndex++;
            }

            //TODO: ANTHONY re-enable all sensors here
            xEventGroupClearBits(xSYSEVENTS_handle, EVENT_RETRIEVE);
        }
        /* Write data to USB if the appropriate flag is set. */
        else if((xEventGroupGetBits(xSYSEVENTS_handle) & EVENT_LOGTOUSB) != 0)
        {

            if(usb_state() == USB_Configured) {
                if(usb_dtr() && packetSize > 0) {
                    do {
                        err = usb_write(&ucRXData, packetSize);
                    } while (err != ERR_NONE && err != ERR_BUSY);
                }
                else {
                    // flush the USB pipes if USB is disconnected
                    usb_flushTx();
                }
            }
         }
    }
}
