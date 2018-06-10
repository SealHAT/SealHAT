/*
 * IncFile1.h
 *
 * Created: 6/7/2018 4:14:47 AM
 *  Author: Anthony Koutroulis
 */ 
 #include "seal_RTOS.h"
 #include "seal_Types.h"
 #include "max30003/ecg.h"
 
#ifndef SEAL_ECG_H_
#define SEAL_ECG_H_

#define ECG_STACK_SIZE  (900 / sizeof(portSTACK_TYPE))	/* number of 32-bit words to reserve for task */
#define ECG_TASK_PRI    (tskIDLE_PRIORITY + 2)

extern TaskHandle_t xECG_th;

int32_t ECG_task_init(void);
void    ECG_task(void *pvParameters);
void    ECG_isr_dataready(void);


#endif /* SEAL_ECG_H_ */