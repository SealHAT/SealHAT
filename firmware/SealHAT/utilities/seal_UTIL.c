#include "seal_UTIL.h"

void i2c_unblock_bus(const uint8_t SDA, const uint8_t SCL)
{
    uint32_t i, count;

    // Set pin SDA direction to input, pull off
    gpio_set_pin_direction(SDA, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(SDA, GPIO_PULL_OFF);
    gpio_set_pin_function(SDA, GPIO_PIN_FUNCTION_OFF);

    for(i = 0, count = 0; i < 50; i++) {
        count += gpio_get_pin_level(SDA);
    }

    if(count < 10) {
        // Set pin SCL direction to output, pull off
        gpio_set_pin_direction(SCL, GPIO_DIRECTION_OUT);
        gpio_set_pin_pull_mode(SCL, GPIO_PULL_OFF);
        gpio_set_pin_function(SCL, GPIO_PIN_FUNCTION_OFF);
    
        // Clock in data until there is no apparent data on the bus
        count = 0;
        while(count < 10) {
            gpio_toggle_pin_level(SCL);
            if (gpio_get_pin_level(SDA)) {
                count++;
            }
        }
    }

}

void set_lowPower_mode(void)
{
    // disable Brown Out Detector
    SUPC->BOD33.reg &= ~SUPC_BOD33_ENABLE;

    /* Select BUCK converter as the main voltage regulator in active mode */
    SUPC->VREG.bit.SEL = SUPC_VREG_SEL_BUCK_Val;
    /* Wait for the regulator switch to be completed */
    while(!(SUPC->STATUS.reg & SUPC_STATUS_VREGRDY));

    /* Set Voltage Regulator Low power Mode Efficiency */
    SUPC->VREG.bit.LPEFF = 0x1;

    /* Apply SAM L21 Erratum 15264 - CPU will freeze on exit from standby if BUCK is disabled */
    SUPC->VREG.bit.RUNSTDBY = 0x1;
    SUPC->VREG.bit.STDBYPL0 = 0x1;

    /* Set Performance Level to PL0 as we run @12MHz */
    _set_performance_level(PM_PLCFG_PLSEL_PL0_Val);
}

const char* seal_strerror(int32_t errnumber)
{
    static const char* const errmsg[]= {"ERR_NONE",         // err code (0)
                                        "ERR_INVALID_DATA", // err code (-1)
                                        "ERR_NO_CHANGE",    // err code (-2)
                                        "ERR_ABORTED",      // err code (-3)
                                        "ERR_BUSY",         // err code (-4)
                                        "ERR_SUSPEND",      // err code (-5)
                                        "ERR_IO",           // err code (-6)
                                        "ERR_REQ_FLUSHED",  // err code (-7)
                                        "ERR_TIMEOUT",      // err code (-8)
                                        "ERR_BAD_DATA",     // err code (-9)
                                        "ERR_NOT_FOUND",    // err code (-10)
                                        "ERR_UNSUPPORTED_DEV", // err code (-11)
                                        "ERR_NO_MEMORY",    // err code (-12)
                                        "ERR_INVALID_ARG",  // err code (-13)
                                        "ERR_BAD_ADDRESS",  // err code (-14)
                                        "ERR_BAD_FORMAT",   // err code (-15)
                                        "ERR_BAD_FRQ",      // err code (-16)
                                        "ERR_DENIED",       // err code (-17)
                                        "ERR_ALREADY_INITIALIZED", // err code (-18)
                                        "ERR_OVERFLOW",         // err code (-19)
                                        "ERR_NOT_INITIALIZED",  // err code (-20)
                                        "ERR_SAMPLERATE_UNAVAILABLE",   // err code (-21)
                                        "ERR_RESOLUTION_UNAVAILABLE",   // err code (-22)
                                        "ERR_BAUDRATE_UNAVAILABLE",     // err code (-23)
                                        "ERR_PACKET_COLLISION",         // err code (-24)
                                        "ERR_PROTOCOL",         // err code (-25)
                                        "ERR_PIN_MUX_INVALID",  // err code (-26)
                                        "ERR_UNSUPPORTED_OP",   // err code (-27)
                                        "ERR_NO_RESOURCE",      // err code (-28)
                                        "ERR_NOT_READY",        // err code (-29)
                                        "ERR_FAILURE",          // err code (-30)
                                        "ERR_WRONG_LENGTH",     // err code (-31)
                                        0};

    if(errnumber < 0 && errnumber >= -31) {
        errnumber *= -1;
    } else {
        errnumber = 32;
    }

    return errmsg[errnumber];
}