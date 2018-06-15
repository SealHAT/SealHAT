#include "sensorconfig.h"
#include <QDebug>

#define STORAGECAPACITY         (8000000000.0)

#define	TOHOUR				3600.0
#define I2C_Speed           1.0/400000
#define SPI_SPEED           1.0/1000000

#define TEMP_CONV_TIME      7.0/1000
#define TEMP_CONV_PWR       90/1000000
#define TEMP_BIT_NUM        16.0
#define TEMP_SB_PWR         0.06/1000000
#define TEMP_I2C_PWR        3.5/1000

#define LIGHT_BIT_NUM       16.0
#define LIGHT_INACT_PWR     0.65/1000000
#define LIGHT_ACT_PWR       0.0000016

#define IMU_SB_PWR          2.0/1000000

#define EKG_I_AVDV          100.0/1000000
#define EKG_I_OV            0.6/1000000
#define EKG_I_SAVDV         0.51/1000000
#define EKG_I_SOV           1.1/1000000
#define EKG_OSCILLATOR      (1.4/1000000)*(24.0)

#define floatDebug() qDebug() << fixed << qSetRealNumberPrecision(10)

#define GPS_ACQ_PWR     30.0/1000
#define GPS_SB_PWR      300.0/1000000

#define SPI_CURRENT     20.0/1000
#define SPI_SB_CURRENT  15.0/1000000

#define MICRO_ACT_PWR   32.0/1000000
#define MICRO_SB_PWR    (13.0)/1000000

uint16_t accFrequency[7]    = {1,10,25,50,100,200,400};
double   acc_actPower[3][7] = {
                               {(3.7/1000000), (5.4/1000000), (8.0/1000000), (12.6/1000000), (22.0/1000000), (40.0/1000000), (75.0/1000000)},
                               {(3.7/1000000), (5.4/1000000), (8.0/1000000), (12.6/1000000), (22.0/1000000), (40.0/1000000), (75.0/1000000)},
                               {(3.7/1000000), (4.4/1000000), (5.6/1000000), (7.7/1000000),(11.7/1000000),(20.0/1000000),(36.0/1000000)}
                              };

uint16_t magFrequency[4] = {1,20,50,100};
double   magPower[2][4]  = {
                            {(100.0/1000000), (200.0/1000000), (475.0/1000000), (950.0/1000000)},
                            {(25.0/1000000), (50.0/1000000), (125.0/1000000), (250.0/1000000)}
                           };

SensorConfig::SensorConfig()
{

}

SensorConfig::SensorConfig(SENSOR_CONFIGS_t cfg)
{
    memoryCount = cfg.num_flash_chips;
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

SENSOR_CONFIGS_t SensorConfig::getSensorConfig()
{
    SENSOR_CONFIGS_t tmp;
    tmp.num_flash_chips = memoryCount;
    tmp.start_day   = start_day;
    tmp.start_hour  = start_hour;
    tmp.start_month = start_month;
    tmp.start_year  = start_year;

    tmp.accConfig = accConfig;
    tmp.magConfig = magConfig;
    tmp.envConfig = envConfig;
    tmp.gpsConfig = gpsConfig;
    tmp.ekgConfig = ekgConfig;

    return tmp;
}

void SensorConfig::setStartTime(QDateTime start)
{
    // possibly use set/get start.toSecsSinceEpoch()
    start_day   = start.date().day();
    start_month = start.date().month();
    start_year  = start.date().year();
    start_hour  = start.time().hour();
}

QDateTime SensorConfig::getStartTime()
{
    // should be UTC but need to coordinate with device clock
    QDateTime(QDate(start_year, start_month, start_day), QTime(start_hour, 0, 0, 0));
}

int SensorConfig::countHours(uint32_t x)
{
    int hours = 0;
    while(x) {
      hours += x & 1;
      x >>= 1;
    }
    return hours;
}

void SensorConfig::calcSampleCount()
{
    int gpsHours = countHours(gpsConfig.activeHour);

    if(gpsHours <= 2) {
        sampleCount[GPS] = (gpsHours * 3600) / 30;
    }
    else {
        sampleCount[GPS] = ((2*3600)/(30) + (gpsHours-2));
    }

    sampleCount[ACCELERATION] = countHours(accConfig.activeHour) * 3600 * 50;
    sampleCount[MAGNET] = countHours(magConfig.activeHour) * 3600 * 20;
    sampleCount[ENVIRONMENTAL] = countHours(envConfig.activeHour) * 3600 * envConfig.period;
    sampleCount[MODULAR] = countHours(ekgConfig.activeHour) * 3600 * (512/(pow(2,(int)ekgConfig.rate)));
}

double SensorConfig::getAvgPowerUse()
{

}

double SensorConfig::getPowerUse()
{
    /*TEMPERATURE POWER*/
    double temp_activePower = ((((3600 - (sampleCount[ENVIRONMENTAL] * TEMP_CONV_TIME)) - (sampleCount[ENVIRONMENTAL] * TEMP_BIT_NUM * I2C_Speed)) * TEMP_SB_PWR)
                        + (sampleCount[ENVIRONMENTAL] * TEMP_CONV_TIME * TEMP_CONV_PWR)
                        + (sampleCount[ENVIRONMENTAL] * TEMP_BIT_NUM * TEMP_I2C_PWR * I2C_Speed)) * countHours(accConfig.activeHour)/3600;
    double temp_inactivePower = (24 - countHours(accConfig.activeHour)) * TEMP_SB_PWR;
    double temp_totalPower = (temp_activePower + temp_inactivePower);   //temp power per day//

    /*LIGHT POWER*/
     double light_activePower = (((3600 - (sampleCount[ENVIRONMENTAL] * LIGHT_BIT_NUM * I2C_Speed)) * LIGHT_INACT_PWR)
                        + (( sampleCount[ENVIRONMENTAL] * LIGHT_BIT_NUM * I2C_Speed) * LIGHT_ACT_PWR)) * countHours(accConfig.activeHour)/3600;
     double light_inactivePower = (24 - countHours(accConfig.activeHour)) * LIGHT_INACT_PWR;
     double light_totalPower = light_activePower + light_inactivePower;   //light power per day//

     /*EKG POWER*/
     double ekg_inactivePower = EKG_I_SAVDV * (24 - countHours(ekgConfig.activeHour)) + (EKG_I_OV * (24 - countHours(ekgConfig.activeHour)));
     double ekg_activePower = (EKG_I_AVDV + EKG_I_OV) * countHours(ekgConfig.activeHour);
     double ekg_totalPower = ekg_inactivePower + ekg_activePower + EKG_OSCILLATOR;

     /*ACCELEROMETER POWER*/
     int acc_tens = (accConfig.opMode/16)%10 - 1; //logic Rethink
     int acc_pwrMode = (accConfig.opMode%16)/4;
     double acc_inactivePower = IMU_SB_PWR * (24 - countHours(accConfig.activeHour));
     double acc_activePower = acc_actPower[acc_pwrMode][acc_tens] * countHours(accConfig.activeHour) ;
     double acc_totalPower = acc_inactivePower + acc_activePower;

     /*MAGNETOMETER POWER*/
     int mag_ones = (magConfig.opMode%16)/4;
     int mag_pwrMode = (magConfig.opMode/16)%10;
     double mag_inactivePower = IMU_SB_PWR*(24 - countHours(magConfig.activeHour));
     double mag_activePower = magPower[mag_pwrMode][mag_ones] * countHours(magConfig.activeHour);
     double mag_totalPower = mag_inactivePower + mag_activePower;

     /*GPS POWER*/
     double gps_activePower;
     double gps_inactivePower = GPS_SB_PWR * (24 - countHours(gpsConfig.activeHour));
     if(gpsConfig.activeHour <= 2){
         gps_activePower = (((GPS_ACQ_PWR/15.0)  + (GPS_SB_PWR* (14.0/15.0))))  * countHours(gpsConfig.activeHour);
     }else{
         gps_activePower = (( (GPS_ACQ_PWR/15.0) + (GPS_SB_PWR* (14.0/15.0)))) * 2 + ((GPS_ACQ_PWR*30.0/3600.0)  + (GPS_SB_PWR* (3300.0)/3600.0))  * (countHours(gpsConfig.activeHour)-2);
     }

     double gps_totalPower = gps_inactivePower + gps_activePower;
     qDebug() << "gps_active hours is " << countHours(gpsConfig.activeHour);
     qDebug() << "gps_activePower is " << gps_activePower;

     /*MEMORY POWER*/  //what is the correct SPI time
     double memory_totalpower = getEstimatedMemoryUse() * SPI_SPEED/3600 * SPI_CURRENT
                         + (24 - (getEstimatedMemoryUse() * SPI_SPEED)/3600) * SPI_SB_CURRENT
                         + (24 * SPI_SB_CURRENT * 3);

     /*MICRO POWER*/
     double micro_tempActiveTime  = sampleCount[ENVIRONMENTAL] * 2 * (I2C_Speed + SPI_SPEED);
     double micro_lightActiveTime = sampleCount[ENVIRONMENTAL] * 2 * (I2C_Speed + SPI_SPEED);
     double micro_accActiveTime   = sampleCount[ACCELERATION] * 6 * (I2C_Speed + SPI_SPEED);
     double micro_magActiveTime   = sampleCount[MAGNET] * 6 * (I2C_Speed + SPI_SPEED);
     double micro_gpsActiveTime   = (sampleCount[GPS] * 100 * I2C_Speed) + (sampleCount[GPS] * 20 * SPI_SPEED);
     double micro_ekgActiveTime   = sampleCount[MODULAR] * 3 * (I2C_Speed + SPI_SPEED);

     double micro_activehour = (micro_tempActiveTime + micro_lightActiveTime + micro_accActiveTime
                                + micro_magActiveTime + micro_gpsActiveTime + micro_ekgActiveTime)*8/3600;
     double micro_totalpower = micro_activehour * MICRO_ACT_PWR * (12) + ((24-micro_activehour) * MICRO_SB_PWR* (12));

    //SUM OF POWER
    return (temp_totalPower + light_totalPower + ekg_totalPower + acc_totalPower + mag_totalPower + gps_totalPower + memory_totalpower + micro_totalpower) * 1000;
}

double SensorConfig::getEstimatedMemoryUse()
{
    unsigned int storage = 0;

    calcSampleCount();

    //Environment size(bytes) of samples per day
    storage += ((4 * ENV_LOG_SIZE) + sizeof(DATA_HEADER_t)) * (sampleCount[ENVIRONMENTAL]/ENV_LOG_SIZE);

    //Accelerometer size(bytes) of samples per day
    storage += ((6 * IMU_LOG_SIZE) + sizeof(DATA_HEADER_t)) * (sampleCount[ACCELERATION]/IMU_LOG_SIZE);

    //Magnetometer size(bytes) of samples per day
    storage += ((6 * IMU_LOG_SIZE) + sizeof(DATA_HEADER_t)) * (sampleCount[MAGNET]/IMU_LOG_SIZE);

    //Gps size(bytes) of samples per day
    storage += ((20 * GPS_LOG_SIZE) + sizeof(DATA_HEADER_t)) * (sampleCount[GPS]/GPS_LOG_SIZE);

    //Ekg size(bytes) of samples per day
    storage += ((3 * ECG_LOG_SIZE) + sizeof(DATA_HEADER_t)) * (sampleCount[MODULAR]/ECG_LOG_SIZE);

    qDebug() << "storage is" << storage;

    return (((double)storage * 90.0)/((double)memoryCount*250000000)) * 100;
}
