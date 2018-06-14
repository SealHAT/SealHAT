#include "sensorconfig.h"

SensorConfig::SensorConfig()
{

}

SensorConfig::SensorConfig(SENSOR_CONFIGS_t cfg)
{
    num_flash_chips = cfg.num_flash_chips;
    start_day   = cfg.start_day;
    start_month = cfg.start_month;
    start_year  = cfg.start_year;
    start_hour  = cfg.start_hour;

    accConfig = cfg.accConfig;
    magConfig = cfg.magConfig;
    envConfig = cfg.envConfig;
    gpsConfig = cfg.gpsConfig;
    ekgConfig = cfg.ekgConfig;
}

void SensorConfig::setStartTime(QDateTime start)
{
    // possibly use set/get start.toSecsSinceEpoch()
    start_day   = start.date().day();
    start_month = start.date().month();
    start_year  = start.date().year();
    start_hour  = start.time().hour();
}

void SensorConfig::setFlashParams(const int chipCount)
{
    num_flash_chips = chipCount;
}

void SensorConfig::setAcceleration(ACC_CFG_t accCfg)
{
    accConfig = accCfg;
}

void SensorConfig::setMagnetometer(MAG_CFG_t magCfg)
{
    magConfig = magCfg;
}

void SensorConfig::setEnvironmental(ENV_CFG_t envCfg)
{
    envConfig = envCfg;
}

void SensorConfig::setGPS(GPS_CFG_t gpsCfg)
{
    gpsConfig = gpsCfg;
}

void SensorConfig::setModular(MOD_CFG_t modCfg)
{
    ekgConfig = modCfg;
}

int countBits(uint32_t x)
{
    int hours = 0;
    while(x) {
      hours += x & 1;
      x >>= 1;
    }
    return hours;
}

float SensorConfig::getAvgPowerUse()
{

}

QTime SensorConfig::getEstimatedRuntime()
{

}

double SensorConfig::getEstimatedMemoryUse()
{
    unsigned int storage = 0;

    //Environment size(bytes) of samples per day
    storage += ((4 * ENV_LOG_SIZE) + sizeof(DATA_HEADER_t)) * (temp_sampleNumber/ENV_LOG_SIZE);

    //Accelerometer size(bytes) of samples per day
    storage += ((6 * IMU_LOG_SIZE) + sizeof(DATA_HEADER_t)) * (acc_sampleNumber/IMU_LOG_SIZE);

    //Magnetometer size(bytes) of samples per day
    storage += ((6 * IMU_LOG_SIZE) + sizeof(DATA_HEADER_t)) * (mag_sampleNumber/IMU_LOG_SIZE);

    //Gps size(bytes) of samples per day
    storage += ((20 * GPS_LOG_SIZE) + sizeof(DATA_HEADER_t)) * (gps_sampleNumber/GPS_LOG_SIZE);

    //Ekg size(bytes) of samples per day
    storage += ((3 * ECG_LOG_SIZE) + sizeof(DATA_HEADER_t)) * (ekg_sampleNumber/ECG_LOG_SIZE);

    storageEst = (templight_storage * templight_groupNum
             + acc_storage * acc_groupNum
             + mag_storage * mag_groupNum
             + gps_storage * gps_groupNum
             + ekg_storage * ekg_groupNum) * 8; //Storage caculate in bits = total Bits

    double StorageConsump = ((double)storageEst * 90.0)/(num_flash_chips*250000000) * 100;
    QString storageconsumpString = " " + QString::number(StorageConsump,'f',2) + " % ";
}
