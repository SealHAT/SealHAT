#ifndef SEALHAT_DEVICE_H
#define SEALHAT_DEVICE_H

#include <QObject>
#include <QTimer>
#include <QQueue>
#include <QByteArray>
#include <QDataStream>
#include <QSerialPort>
#include <QSerialPortInfo>
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


    // check how many sensor samples are ready for consumption
    int queueSize();
    // get the next sensor sample
    SensorSample queuePop();

private:
    // parses the data in the clean_data array to find data headers
    void deserializeDataPackets();
    // takes header info and stream to create sensor packets
    void parseSensorPackets(QDataStream& stream, quint8 device, quint32 time, quint8 seqNum, quint16 length);

signals:
    // signal emitted to indicate the device has connected
    void sealhat_connected();
    // signal emitted to indicate the device has disconnected
    void sealhat_disconnected();
    // signal emitted with data from the device
    void data_in(QByteArray data);

public slots:
    // disconnect from the device
    void disconnectFromDevice();

private slots:
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);

private:
     QSerialPort          sealhat;      // serial object for connecting to device
     QTimer               pollTimer;    // timer to poll for incoming data
     QByteArray           in_data;      // Data from device before CRC checks
     QByteArray           clean_data;   // Data from device after CRC checks
//     QDataStream*         stream;       // data stream to deserialize the cleaned data
     QQueue<SensorSample> data_q;       // queue of processed sensor readings
};

#endif // SEALHAT_DEVICE_H
