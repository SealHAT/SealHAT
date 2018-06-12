#include "seal_RTOS.h"
#include "tasks/seal_ECG.h"
#include "tasks/seal_ENV.h"
#include "tasks/seal_IMU.h"
#include "tasks/seal_CTRL.h"
#include "tasks/seal_GPS.h"
#include "tasks/seal_DATA.h"
#include "tasks/seal_SERIAL.h"

int main(void)
{
    // clear the I2C buses. I2C devices can lock up the bus if there was a reset during a transaction.
    i2c_unblock_bus(ENV_SDA, ENV_SCL);
    i2c_unblock_bus(GPS_SDA, GPS_SCL);
    i2c_unblock_bus(IMU_SDA, IMU_SCL);

    // set FLASH CS pins HIGH
    gpio_set_pin_level(MEM_CS0, true);
    gpio_set_pin_level(MEM_CS1, true);
    gpio_set_pin_level(MEM_CS2, true);

    // initialize the system and set low power mode
    system_init();
    set_lowPower_mode();

    // enable the calendar driver. this function ALWAYS returns ERR_NONE.
    calendar_enable(&RTC_CALENDAR);

    // check what caused the reset and log as necessary. return value is not needed (handled in function)
    checkResetReason();

    // start the data aggregation task
    if(DATA_task_init() != ERR_NONE) {
        while(1) {;}
    }

    // start the environmental sensors
    if(ENV_task_init(1) != ERR_NONE) {
        while(1) {;}
    }

    // start the ECG
    if(ECG_task_init() != ERR_NONE) {
        // will never work if there is no device, shouldn't go into a while(1)
        gpio_set_pin_level(LED_RED, false);
        delay_ms(100);
        gpio_set_pin_level(LED_RED, true);
    }

    // GPS task init
    if(GPS_task_init(0) != ERR_NONE) {
        while(1) {;}
    }

    // IMU task init.
    if(IMU_task_init(ACC_SCALE_2G, ACC_HR_50_HZ, MAG_LP_20_HZ) != ERR_NONE) {
        while(1) {;}
    }

    // SERIAL task init.
    if(SERIAL_task_init() != ERR_NONE) {
        while(1) {;}
    }

    // start the control task.
    if(CTRL_task_init() != ERR_NONE) {
        while(1) {;}
    }

    // Start the freeRTOS scheduler, this will never return.
    vTaskStartScheduler();
    return 0;
}
