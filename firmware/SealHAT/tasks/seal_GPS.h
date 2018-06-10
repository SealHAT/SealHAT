/*
 * seal_GPS.h
 *
 * Created: 5/11/2018 2:35:55 PM
 *  Author: Anthony Koutroulis
 */
 #include "seal_RTOS.h"
 #include "seal_CTRL.h"
 #include "seal_DATA.h"
 #include "sam-m8q/gps.h"

#ifndef SEAL_GPS_H_
#define SEAL_GPS_H_

#define GPS_STACK_SIZE  (1900 / sizeof(portSTACK_TYPE))	/* number of 32-bit words to reserve for task */
#define GPS_TASK_PRI    (tskIDLE_PRIORITY + 3)
#define GPS_MOVINGTIME  ()
#define GPS_MAXMOVE     (120) /* maximum time in seconds to permit high resolution movement per day  */

typedef enum GPS_NOTIFY_VALS {
    GPS_NOTIFY_NONE     = 0x00000000,
    GPS_NOTIFY_TXRDY    = 0x00000001,
    GPS_NOTIFY_MOTION   = 0x00000002,
    GPS_NOTIFY_REVERT   = 0x00000004,
    GPS_NOTIFY_HOUR     = 0x00000008,
    GPS_NOTIFY_ALL      = 0xFFFFFFFF
} GPS_NOTIFY_VALS;

extern TaskHandle_t xGPS_th;

int32_t GPS_task_init(void *profile);   // TODO restrict to enumerated type or struct
void    GPS_task(void *pvParameters);
void    GPS_isr_dataready(void);
void    GPS_movement_cb(TimerHandle_t xTimer);
int32_t GPS_log(GPS_MSG_t *msg, const int32_t ERR, const DEVICE_ERR_CODES_t ERR_CODES);

#endif /* SEAL_GPS_H_ */
