#ifndef LSM303CONFIG_H
#define LSM303CONFIG_H

#include "seal_Types.h"

class Lsm303Config
{
    public:
        Lsm303Config();
        Lsm303Config(ACC_CFG_t cfg);

    private:
        ACC_CFG_t   accConfig;      // configuration data for the accelerometer
};

#endif // LSM303CONFIG_H
