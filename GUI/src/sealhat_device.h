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
    bool sendConfig();


    // check how many sensor samples are ready for consumption
    int queueSize();
    // get the next sensor sample
    SensorSample queuePop();

private:
    // parses the data in the clean_data array to find data headers
    void deserializeDataPackets();
    // takes header info and stream to create sensor packets
    void parseSensorPackets(QDataStream& startStream, quint8 device, quint32 time, quint8 seqNum, quint16 length);

    /**** Harware specific parsing functions ****/
    void parse_si7051(QDataStream& startStream, quint32 time, quint8 seqNum, quint16 length);
    void parse_max44009(QDataStream& startStream, quint32 time, quint8 seqNum, quint16 length);
    void parse_lsm303agr_acc(QDataStream& startStream, quint32 time, quint8 seqNum, quint16 length);
    void parse_lsm303agr_mag(QDataStream& startStream, quint32 time, quint8 seqNum, quint16 length);
    void parse_samm8q(QDataStream& startStream, quint32 time, quint8 seqNum, quint16 length);
    void parse_max30003(QDataStream& startStream, quint32 time, quint8 seqNum, quint16 length);

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
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);

private:
     QSerialPort          sealhat;      // serial object for connecting to device
     SENSOR_CONFIGS_t     devCfg; // current devic configuration
     QTimer               pollTimer;    // timer to poll for incoming data
     QByteArray           in_data;      // Data from device before CRC checks
     QByteArray           clean_data;   // Data from device after CRC checks
     QQueue<SensorSample> data_q;       // queue of processed sensor readings
     QFile                rawDataLog;   // file to log raw data from device for debug
};

QDataStream& operator>>(QDataStream& stream, DATA_TRANSMISSION_t& txData);
QDataStream& operator>>(QDataStream& stream, DATA_HEADER_t& data_header);

#endif // SEALHAT_DEVICE_H
