#include "sensorsample.h"

SensorSample::SensorSample()
{ /* EMPTY */ }

SensorSample::SensorSample(DEVICE_ID_t newType, QDateTime newTime, unsigned int seqNum, SENSOR_DATA_t newData)
{
    this->set_data(newType, newTime, seqNum, newData);
}

bool SensorSample::set_data(DEVICE_ID_t newType, QDateTime newTime, unsigned int seqNum, SENSOR_DATA_t newData)
{
    this->sensorType   = newType;
    this->timestamp    = newTime;
    this->packetNumber = seqNum;
    this->data         = newData;
}

QString SensorSample::get_csv()
{
    // order: sensor type, date, time, data1, data2, data3
    QString csv("");
    csv += this->get_typeStr();
    csv += QString(",");
    csv += QString("%1,").arg(this->packetNumber);
    csv += this->timestamp.date().toString("dd.MM.yyyy");
    csv += QString(",");
    csv += this->timestamp.time().toString("hh:mm:ss.zzz");
    csv += QString(",");
    csv += this->get_SI();

    // add blank field(s) for single and double data point sensors
    if(!(this->sensorType == DEVICE_ID_ACCELEROMETER
         || this->sensorType == DEVICE_ID_MAGNETIC_FIELD
         || this->sensorType == DEVICE_ID_GYROSCOPE
         || this->sensorType == DEVICE_ID_GPS)) {
        csv += QString(",,");
    }
    else if(this->sensorType == DEVICE_ID_GPS) {
        csv += QString(",");
    }

    return csv;
}

QString SensorSample::get_SI()
{
    QString value;

    switch(this->sensorType) {
        case DEVICE_ID_LIGHT          : value = QString::number(data.light, 'f');
                break;
        case DEVICE_ID_TEMPERATURE    : value = QString::number(data.temperature, 'f');
                break;
        case DEVICE_ID_ACCELEROMETER  : value = QString("%1,%2,%3").arg(data.acceleration.xAxis, 0, 'f', 6).arg(data.acceleration.yAxis, 0, 'f', 6).arg(data.acceleration.zAxis, 0, 'f', 6);
                break;
        case DEVICE_ID_MAGNETIC_FIELD : value = QString("%1,%2,%3").arg(data.magnetic.xAxis, 0, 'f', 6).arg(data.magnetic.yAxis, 0, 'f', 6).arg(data.magnetic.zAxis, 0, 'f', 6);
                break;
        case DEVICE_ID_GYROSCOPE      : value = QString("Err: no gyroscope code yet");
                break;
        case DEVICE_ID_PRESSURE       : value = QString("Err: no pressure code yet");
                break;
        case DEVICE_ID_DEPTH          : value = QString("Err: no depth code yet");
                break;
        case DEVICE_ID_GPS            : value = QString::number(data.geeps.time.year) + ":" + QString::number(data.geeps.time.month) + ":" + QString::number(data.geeps.time.day) + ":" +
                                                QString::number(data.geeps.time.hour) + ":" + QString::number(data.geeps.time.minute) + ":" + QString::number(data.geeps.time.second) + "," +
                                                QString::number(data.geeps.position.lon) + " " + QString::number(data.geeps.position.lat);    //TODO: format time and position to taste
                break;
        case DEVICE_ID_EKG            : value = QString::number(data.voltage, 'f');
                break;
        case DEVICE_ID_SYSTEM         : value = QString("Err: no SYS code yet");
                break;
        default : value = QString("Err: invalid sensor type!");
    };
    return value;
}

DEVICE_ID_t SensorSample::get_type()
{
    return this->sensorType;
}

QString SensorSample::get_typeStr()
{
    QString value;

    switch(this->sensorType) {
        case DEVICE_ID_LIGHT          : value = QString("light");
                break;
        case DEVICE_ID_TEMPERATURE    : value = QString("temperature");
                break;
        case DEVICE_ID_ACCELEROMETER  : value = QString("acceleration");
                break;
        case DEVICE_ID_MAGNETIC_FIELD : value = QString("magnetic");
                break;
        case DEVICE_ID_GYROSCOPE      : value = QString("gyro");
                break;
        case DEVICE_ID_PRESSURE       : value = QString("pressure");
                break;
        case DEVICE_ID_DEPTH          : value = QString("depth");
                break;
        case DEVICE_ID_GPS            : value = QString("gps");
                break;
        case DEVICE_ID_EKG            : value = QString("ekg");
                break;
        case DEVICE_ID_SYSTEM         : value = QString("Err: invalid sensor type!");
                break;
        default : value = QString("Err: invalid sensor type!");
    };
    return value;
}
