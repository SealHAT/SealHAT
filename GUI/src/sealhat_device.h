#ifndef SEALHAT_DEVICE_H
#define SEALHAT_DEVICE_H

#include <QObject>
#include <QTimer>
#include <QQueue>
#include <QByteArray>
#include <QDataStream>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QFile>
#include "seal_Types.h"
#include "sensorsample.h"
#include "crc32.h"

class SealHAT_device : public QObject
{
    Q_OBJECT
public:
    // constructor
    explicit SealHAT_device(QObject *parent = nullptr);
    // destructor
    ~SealHAT_device();

    // connect to the SealHAT device
    bool connectToDevice(QString portName);
    // enqueue data to send
    int sendData(QByteArray data);
    // start streaming data over USB
    bool startStream();
    // Stop USB streaming
    void stopStream();
    // start a flash download over USB
    bool download();
    // get the current device configuration
    bool getConfig();
    // send configuration to the device
    bool sendConfig(SENSOR_CONFIGS_t newConfigs);

    // check how many sensor samples are ready for consumption
    int queueSize();
    // get the next sensor sample
    SensorSample queuePop();

private:
    // parses the data in the clean_data array to find data headers
    void deserializeDataPackets();

    /**** Harware specific parsing functions ****/
    void parse_si7051(QDataStream& startStream, DATA_HEADER_t header);
    void parse_max44009(QDataStream& startStream, DATA_HEADER_t header);
    void parse_lsm303agr_acc(QDataStream& startStream, DATA_HEADER_t header);
    void parse_lsm303agr_mag(QDataStream& startStream, DATA_HEADER_t header);
    void parse_samm8q(QDataStream& startStream, DATA_HEADER_t header);
    void parse_max30003(QDataStream& startStream, DATA_HEADER_t header);

signals:
    // signal emitted to indicate the device has connected
    void sealhat_connected();
    // signal emitted to indicate the device has disconnected
    void sealhat_disconnected();
    // signal emitted with data from the device
    void samplesReady(QQueue<SensorSample>* q);

public slots:
    // disconnect from the device
    void disconnectFromDevice();

private slots:
    void StreamingReadyRead_cb();
    void SerialTimerTimout_cb();
    void SerialError_cb(QSerialPort::SerialPortError error);

private:
     QSerialPort          sealhat;      // serial object for connecting to device
     SENSOR_CONFIGS_t     devCfg;       // current devic configuration
     QTimer               pollTimer;    // timer to poll for incoming data
     QByteArray           in_data;      // Data from device before CRC checks
     QByteArray           clean_data;   // Data from device after CRC checks
     QQueue<SensorSample> data_q;       // queue of processed sensor readings
     QFile                rawDataLog;   // file to log raw data from device for debug
     QFile                preCRCfile;   // file to log raw data prior to CRC check
     Crc32                crc32;        // crc32 class
};

QDataStream& operator>>(QDataStream& stream, DATA_HEADER_t& header);
QDataStream& operator>>(QDataStream& stream, SENSOR_CONFIGS_t& sensorCfg);
QDataStream& operator>>(QDataStream& stream, SYSTEM_CONFIG_t& sysCfg);
QDataStream& operator>>(QDataStream& stream, DATA_TRANSMISSION_t& txData);
QDataStream& operator>>(QDataStream& stream, gps_log_t& gpsLog);
QDataStream& operator>>(QDataStream& stream, min_pvt_t& gpsPosition);
QDataStream& operator>>(QDataStream& stream, utc_time_t& gpsTime);

#endif // SEALHAT_DEVICE_H
