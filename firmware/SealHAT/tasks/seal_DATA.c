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

static SemaphoreHandle_t DATA_mutex;                                // mutex to control access to USB terminal
static StaticSemaphore_t xDATA_mutexBuff;                           // static memory for the mutex

static StreamBufferHandle_t xDATA_sb;                               // stream buffer for getting data into FLASH or USB
static uint8_t              dataQueueStorage[DATA_QUEUE_LENGTH];    // static memory for the data queue
static StaticStreamBuffer_t xDataQueueStruct;                       // static memory for data queue data structure

FLASH_DESCRIPTOR seal_flash_descriptor;                             /* Declare flash descriptor. */
//DATA_TRANSMISSION_t usbPacket;

int32_t ctrlLog_write(uint8_t* buff, const uint32_t LEN)
{
    uint32_t err;

    if(xSemaphoreTake(DATA_mutex, ~0)) {

        // bail early if there isn't enough space
        portENTER_CRITICAL();
        if(xStreamBufferSpacesAvailable(xDATA_sb) >= LEN) {
            err = xStreamBufferSend(xDATA_sb, buff, LEN, 0);
        }
        else {
            err = ERR_NO_RESOURCE;
            gpio_set_pin_level(LED_RED, false);
        }
        portEXIT_CRITICAL();

        xSemaphoreGive(DATA_mutex);
    }
    else {
        err = ERR_FAILURE;
        gpio_set_pin_level(LED_RED, false);
    }

    return err;
}

int32_t ctrlLog_writeISR(uint8_t* buff, const uint32_t LEN)
{
    uint32_t err;

    if(xSemaphoreTake(DATA_mutex, ~0)) {

        // bail early if there isn't enough space
        portENTER_CRITICAL();
        if(xStreamBufferSpacesAvailable(xDATA_sb) >= LEN) {
            err = xStreamBufferSendFromISR(xDATA_sb, buff, LEN, 0);
        }
        else {
            err = ERR_NO_RESOURCE;
            gpio_set_pin_level(LED_RED, false);
        }
        portEXIT_CRITICAL();

        xSemaphoreGive(DATA_mutex);
    }
    else {
        err = ERR_FAILURE;
        gpio_set_pin_level(LED_RED, false);
    }

    return err;
}

int32_t DATA_task_init(void)
{
    // enable CRC generator. This function does nothing apparently,
    // but we call it to remain consistent with API. it ALWAYS returns ERR_NONE.
    crc_sync_enable(&CRC_0);

    // create the mutex for access to the data queue
    DATA_mutex = xSemaphoreCreateMutexStatic(&xDATA_mutexBuff);
    configASSERT(DATA_mutex);

    // create the data queue
    xDATA_sb = xStreamBufferCreateStatic(DATA_QUEUE_LENGTH, PAGE_SIZE_LESS, dataQueueStorage, &xDataQueueStruct);
    configASSERT(xDATA_sb);

    /* Read stored device settings from EEPROM and make them accessible to all devices. */
    if (eeprom_read_configs(&eeprom_data)) {
        return ERR_BAD_ADDRESS;
    }

    /* Initialize flash device(s). */
    //flash_io_init(&seal_flash_descriptor, PAGE_SIZE_LESS);

    xDATA_th = xTaskCreateStatic(DATA_task, "DATA", DATA_STACK_SIZE, NULL, DATA_TASK_PRI, xDATA_stack, &xDATA_taskbuf);
    configASSERT(xDATA_th);

    return ERR_NONE;
}

void DATA_task(void* pvParameters)
{
    int32_t err;
    (void)pvParameters;
    static DATA_TRANSMISSION_t usbPacket;

    /* Receive and write data forever. */
    for(;;)
    {
        /* Receive a page worth of data. */
        xStreamBufferReceive(xDATA_sb, usbPacket.data, PAGE_SIZE_LESS, portMAX_DELAY);

        /* Write data to USB if the appropriate flag is set. */
        if((xEventGroupGetBits(xSYSEVENTS_handle) & EVENT_LOGTOUSB) != 0)
        {
            // setup the packet header and CRC start value, then perform CRC32
            usbPacket.startSymbol = USB_PACKET_START_SYM;
            usbPacket.crc = 0xFFFFFFFF;
            crc_sync_crc32(&CRC_0, (uint32_t*)usbPacket.data, PAGE_SIZE_LESS/sizeof(uint32_t), &usbPacket.crc);

            // complement CRC to match standard CRC32 implementations
            usbPacket.crc ^= 0xFFFFFFFF;

            if(usb_state() == USB_Configured) {
                if(usb_dtr()) {
                    err = usb_write(&usbPacket, sizeof(DATA_TRANSMISSION_t));
                    if(err != ERR_NONE && err != ERR_BUSY) {
                        // TODO: log usb errors, however rare they are
                        gpio_set_pin_level(LED_GREEN, false);
                    }
                }
                else {
                    usb_flushTx();
                }
            }
         }
         
         /* Log data to flash if the appropriate flag is set. */
         if((xEventGroupGetBits(xSYSEVENTS_handle) & EVENT_LOGTOFLASH) != 0)
         {
             /* Write data to external flash device. */
             //flash_io_write(&seal_flash_descriptor, usbPacket.data, PAGE_SIZE_LESS);
         }       
    }
}
