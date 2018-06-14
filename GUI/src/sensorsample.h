#ifndef SENSORSAMPLE_H
#define SENSORSAMPLE_H

#include <QByteArray>
#include <Qstring>
#include <QDateTime>
#include "seal_Types.h"

typedef union {
     float data[4];
     AxesSI_t        acceleration;         /**< acceleration values are in milliGravities (mG) */
     AxesSI_t        magnetic;             /**< magnetic vector values are in micro-Tesla (uT) */
     AxesSI_t        orientation;          /**< orientation values are in degrees */
     AxesSI_t        gyro;                 /**< gyroscope values are in rad/s */
     gps_log_t       geeps;
     float           temperature;          /**< temperature is in degrees centigrade (Celsius) */
     float           distance;             /**< distance in centimeters */
     float           light;                /**< light in SI lux units */
     float           pressure;             /**< pressure in hectopascal (hPa) */
     float           relative_humidity;    /**< relative humidity in percent */
     float           current;              /**< current in milliamps (mA) */
     float           voltage;              /**< voltage in volts (V) */
} SENSOR_DATA_t;

class SensorSample
{
    public:
        // constructors
        SensorSample();
        SensorSample(DEVICE_ID_t newType, QDateTime newTime, unsigned int seqNum, SENSOR_DATA_t newData);

        // set the sensor sample data
        bool set_data(DEVICE_ID_t newType, QDateTime newTime, unsigned int seqNum, SENSOR_DATA_t newData);

        // get the sample data in csv form
        QString     get_csv();
        // get the sensor sample as a single (or tripple for 3-axis sensors) SI float value
        QString     get_SI();
        // get the type of sensor as a string
        QString     get_typeStr();
        // get the type of sensor as an enum
        DEVICE_ID_t get_type();

    private:
       DEVICE_ID_t      sensorType;     // sensor type ID
       HARDWARE_ID_t    hardwareID;     // specific hardware used to gather data
       QDateTime        timestamp;      // timestamp of this sample
       unsigned int     packetNumber;   // packet number from this sensor
       SENSOR_DATA_t    data;           // sensor data as a union
};

#endif // SENSORSAMPLE_H
