/**
 * tick_lowpower.c
 *
 * Created 30-May-2018
 * Author: Ethan Slattery
 * Sources:
 *      Atmel Start Tickless Idle example
 *      https://yurovsky.github.io/2015/04/03/freertos-tickless-low-power-m0.html
 *      https://yurovsky.github.io/2015/04/09/freertos-low-power-samd20.html
 */

#include "driver_init.h"
#include "seal_RTOS.h"
#include "FreeRTOSConfig.h"

#define USE_CUSTOM_SYSTICK 1

// #define TIMER_HZ (CONF_GCLK_RTC_FREQUENCY / CONF_TC4_PRESCALE)  // Frequency of timer
// #define TIMER_COUNTS_ONE_TICK (TIMER_HZ / configTICK_RATE_HZ)   // Value per os tick of timer
// #define TIMER_INTERVAL_TICK 1               // Tick period in millisecond
// #define TIMER_MAX_COUNT (0xFFFFFFFFUL)      // Max value of a timer

/* Maximum possible suppressed ticks with timer */
#define TIMER_MAX_SUPPRESSED_TICKS (TIMER_MAX_COUNT / TIMER_COUNTS_ONE_TICK)

/* External declaration of freeRTOS SysTick handler */
extern void xPortSysTickHandler(void);

/* Function for setting up timer */
void vPortSetupTimerInterrupt(void);

static bool tickEnabled = false;

/** @brief timer callback to invoke the systick
 * @param timer_task [IN] Pointer to timer_task structure
 */
void lowpower_systick(void)
{
    if(tickEnabled) {
        xPortSysTickHandler();
    }
}

void disable_freeRTOS_systick(void)
{
    hri_rtcmode0_clear_INTEN_reg(RTC, RTC_PERIODIC_INTERRUPT_SYSTICK);
}

/** @brief Initialize and start timer for tick
 *
 * Set up TC4 for use as the tick timer, since TC4 is the only
 * timer that works in standby sleep mode. This same timer can be
 * used for tickless sleep. The function in port.c must be weak
 * aliased for this function to work.
 */
 #if USE_CUSTOM_SYSTICK > 0
void vPortSetupTimerInterrupt(void)
{
    tickEnabled = true;

    // clear out any old interrupts and then enable the systick interrrupt
    hri_rtcmode0_clear_INTFLAG_reg(RTC, RTC_MODE0_INTFLAG_PER_Msk);
    hri_rtcmode0_clear_INTEN_reg(RTC, RTC_MODE0_INTFLAG_PER_Msk);

    // set the RTOS tick interrupt generator
    hri_rtcmode0_set_INTEN_reg(RTC, RTC_PERIODIC_INTERRUPT_SYSTICK);
}
#endif

/** @brief Tickless idle using TC4 on M0+
 *
 * Function to configure timer for sleep, and calculate time slept.
 * @param xExpectedIdleTime [IN] the number of ticks task want to sleep.
 */
#if configUSE_TICKLESS_IDLE > 0
void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    (void)xExpectedIdleTime;
// 	/* Reconfigure the timer to act as sleep timer */
// 	TIMER_0_task.cb = systick_lowpower_empty_cb;
//
// 	/* Check that the offset is not greater than the range of the timer */
// 	if (xExpectedIdleTime > TIMER_MAX_SUPPRESSED_TICKS) {
// 		xExpectedIdleTime = TIMER_MAX_SUPPRESSED_TICKS;
// 	}
//
// 	/* Set sleep time, -1 because we want to wake up before the last tick */
// 	TIMER_0_task.interval = (xExpectedIdleTime - 1) * TIMER_INTERVAL_TICK;
//
// 	/* Check if we still should sleep */
// 	if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
// 		/* Reset the timer to act as SysTick */
// 		TIMER_0_task.cb       = lowpower_systick;
// 		TIMER_0_task.interval = TIMER_INTERVAL_TICK;
// 	} else {
// 		if (xExpectedIdleTime > 0) {
// 			/* Data sync barrier before sleep */
// 			//__DSB();
// 			/* Go to sleep */
// 			//__WFI();
//             sleep(PM_SLEEPCFG_SLEEPMODE_STANDBY_Val);
//
// 			/* Reset counter to less than one os tick */
// 			vTaskStepTick(TIMER_MS.time);
// 			TIMER_MS.time = 0;
// 		}
// 		/* Reset the timer to act as SysTick */
// 		TIMER_0_task.cb       = lowpower_systick;
// 		TIMER_0_task.interval = TIMER_INTERVAL_TICK;
//
// 		/* Make sure that the counter hasn't passed the CC before callback was registered */
// 		if (TIMER_MS.time > TIMER_INTERVAL_TICK) {
// 			/* If so, reload count value, and step one tick	*/
// 			vTaskStepTick(1);
// 		}
// 	}
}
#endif