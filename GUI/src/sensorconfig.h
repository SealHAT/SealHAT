#ifndef SENSORCONFIG_H
#define SENSORCONFIG_H

#include <QTime>
#include "seal_Types.h"

class SensorConfig
{
    public:
        SensorConfig();
        SensorConfig(SENSOR_CONFIGS_t cfg);

        void setStartTime(QDateTime start);
        void setMemoryCount(const int chipCount) { memoryCount = chipCount; }
        void setAcceleration(ACC_CFG_t accCfg)  { accConfig = accCfg; }
        void setMagnetometer(MAG_CFG_t magCfg)  { magConfig = magCfg; }
        void setEnvironmental(ENV_CFG_t envCfg) { envConfig = envCfg; }
        void setGPS(GPS_CFG_t gpsCfg) { gpsConfig = gpsCfg; }
        void setModular(MOD_CFG_t modCfg) { ekgConfig = modCfg; }

        QDateTime getStartTime();
        int       getChipCount() { return memoryCount; }
        ACC_CFG_t getAccelConfig() { return accConfig; }
        MAG_CFG_t getMagConfig() { return magConfig; }
        ENV_CFG_t getEnvConfig() { return envConfig; }
        GPS_CFG_t getGPSConfig() { return gpsConfig; }
        MOD_CFG_t getModularConfig() { return modConfig; }

        float  getAvgPowerUse();
        QTime  getEstimatedRuntime();
        double getEstimatedMemoryUse();

    protected:
        int countBits(uint32_t x);

    private:
        enum sensor{ACCELERATION, MAGNET, ENVIRONMENTAL, GPS, MODULAR, SENSOR_COUNT};
        unsigned int storage[SENSOR_COUNT];
        unsigned int storage[SENSOR_COUNT];

        quint8      memoryCount;         // number of flash chips installed on device
        quint8      start_day;           // range from 1 to 28/29/30/31
        quint8      start_month;         // range from 1 to 12
        quint16     start_year;          // absolute year >= 1970
        quint8      start_hour;          // hour of the day the device will start logging. range 0-23

        ACC_CFG_t   accConfig;           // configuration data for the accelerometer
        MAG_CFG_t   magConfig;           // configuration data for the magnetometer
        ENV_CFG_t   envConfig;           // configuration data for the temperature sensor
        GPS_CFG_t   gpsConfig;           // configuration data for the GPS
        MOD_CFG_t   ekgConfig;           // configuration data for the EKG

};

#endif // SENSORCONFIG_H
